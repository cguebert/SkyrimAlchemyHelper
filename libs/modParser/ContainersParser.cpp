#include "ContainersParser.h"
#include "zlib/zlib.h"

#include <iostream>

namespace modParser
{

using namespace std;

ContainersParser::ContainersParser(const std::string& fileName, const std::string& language, const std::vector<std::string>& modsNames)
	: Mod(fileName, language)
	, m_modsNames(modsNames)
{
	// We want to read the masters list now
	parsePluginInformation();

	int nbMasters = m_masters.size();
	m_mastersIds.assign(nbMasters + 1, -1);
	for (int i = 0; i < nbMasters; ++i)
	{
		auto it = find(m_modsNames.begin(), m_modsNames.end(), m_masters[i]);
		if (it != m_modsNames.end())
			m_mastersIds[i] = it - m_modsNames.begin();
	}

	auto it = find(m_modsNames.begin(), m_modsNames.end(), m_modName);
	if (it != m_modsNames.end())
		m_mastersIds[nbMasters] = it - m_modsNames.begin();
}

void ContainersParser::registerContainersParsers()
{
	m_groupParsers.clear();

// REFR
	RecordParser refrRecord;
	refrRecord.type = "REFR";
	refrRecord.beginFunction = [this](uint32_t id, uint32_t /*dataSize*/, uint32_t /*flags*/) {
		auto it = find_if(m_containers.begin(), m_containers.end(), [id](const Container& c){
			return c.id2 == id;
		});
		if (it == m_containers.end() || !m_currentLocation)
			return false;
		m_currentId = it - m_containers.begin();
		it->cell = m_currentLocation;
		it->interior = m_isInteriorCell;
		return true;
	};

	refrRecord.endFunction = [this](uint32_t id) {
		const auto& container = m_containers[m_currentId];
	//	cout << hex << uppercase << container.id << " " << container.base << " " << container.cell << dec << endl;
	};

	refrRecord.fields.emplace_back("NAME", [this](uint16_t dataSize) {
		in >> m_containers[m_currentId].base;
	});

// CELL
	RecordParser cellRecord;
	cellRecord.type = "CELL";
	cellRecord.beginFunction = [this](uint32_t id, uint32_t /*dataSize*/, uint32_t /*flags*/) {
		m_currentLocation = id;
		m_subGroupsDepth = 0;
		m_isInteriorCell = true;
		return false;
	};

	GroupParser cellGroup("CELL",{ refrRecord, cellRecord });
	cellGroup.beginSubGroupFunction = [this]() {
		++m_subGroupsDepth;
	};

	cellGroup.endSubGroupFunction = [this]() {
		if (!m_subGroupsDepth)
			m_currentLocation = 0;
		--m_subGroupsDepth;
	};

	m_groupParsers.push_back(cellGroup);

// WRLD
	RecordParser worldRecord;
	worldRecord.type = "WRLD";
	worldRecord.beginFunction = [this](uint32_t id, uint32_t /*dataSize*/, uint32_t /*flags*/) {
		m_currentLocation = id;
		m_subGroupsDepth = 0;
		m_isInteriorCell = false;
		return false;
	};

	GroupParser worldGroup("WRLD", { refrRecord, worldRecord });
	worldGroup.beginSubGroupFunction = [this]() {
		++m_subGroupsDepth;
	};

	worldGroup.endSubGroupFunction = [this]() {
		if (!m_subGroupsDepth)
			m_currentLocation = 0;
		--m_subGroupsDepth;
	};

	m_groupParsers.push_back(worldGroup);
}

ContainersParser::Containers ContainersParser::findContainers(const std::vector<uint32_t>& ids)
{
	registerContainersParsers();

	const int nb = ids.size();
	m_containers.clear();
	m_containers.resize(nb);

	uint32_t nbMasters = m_masters.size();
	uint32_t mask = nbMasters << 24;
	for (int i = 0; i < nb; ++i)
	{
		auto& container = m_containers[i];
		const auto& id = ids[i];

		container.id = id;
		container.id2 = (id & 0x00FFFFFF) | mask;
	}

	doParse();

	// Convert FormIDs from this mod to the complete mods list
	for (auto& container : m_containers)
	{
		int8_t masterId = std::min(container.base >> 24, nbMasters);
		int32_t modMask = m_mastersIds[masterId] << 24;
		container.base = (container.base & 0x00FFFFFF) | modMask;

		masterId = container.cell >> 24;
		modMask = m_mastersIds[masterId] << 24;
		container.cell = (container.cell & 0x00FFFFFF) | modMask;
	}

	return m_containers;
}

void ContainersParser::registerCellsParsers()
{
	RecordParser cellRecord;
	cellRecord.type = "CELL";
	cellRecord.beginFunction = [this](uint32_t id, uint32_t dataSize, uint32_t flags) {
		auto it = find_if(m_cells.begin(), m_cells.end(), [id](const Cell& c){
			return c.id2 == id;
		});
		if (it == m_cells.end())
			return false;
		m_currentId = it - m_cells.begin();

		uint32_t size = dataSize;
		parser::Parser parser;
		parser::Parser* pIn = &in;
		if (flags & 0x40000) // Compressed
		{
			uint32_t decompSize;
			in >> decompSize;
			std::vector<unsigned char> compressedData;
			compressedData.resize(dataSize - 4);
			in >> compressedData;

			string decompressedData;
			decompressedData.resize(decompSize);

			uLongf decSize = decompSize;
			uncompress(reinterpret_cast<Bytef*>(&decompressedData[0]), &decSize, compressedData.data(), dataSize - 4);
			istringstream ss(decompressedData);
			parser.setStream(move(ss));

			pIn = &parser;
			size = decompSize;
		}

		auto start = pIn->tellg();
		while (pIn->tellg() - start < size)
		{
			string type = pIn->readString(4);
			uint16_t fieldSize;
			(*pIn) >> fieldSize;
			if (type == "FULL")
			{
				cout << hex << uppercase << id << dec << endl;
				if (!m_useStringsTable) // TODO: LString !
				{
					it->cellName = pIn->readZString();
					cout << it->cellName << endl;
				}
				else
					pIn->jump(fieldSize);
			}
			else
				pIn->jump(fieldSize);
		}

		return false;
	};

	GroupParser cellGroup("CELL", { cellRecord });

	m_groupParsers.push_back(cellGroup);
}

ContainersParser::Cells ContainersParser::getCellsName(const std::vector<uint32_t>& ids)
{
	m_groupParsers.clear();

	registerCellsParsers();

	const int nb = ids.size();
	m_cells.clear();
	m_cells.resize(nb);

	int nbMasters = m_masters.size();
	uint32_t mask = nbMasters << 24;
	for (int i = 0; i < nb; ++i)
	{
		auto& cell = m_cells[i];
		const auto& id = ids[i];

		cell.id = id;
		cell.id2 = (id & 0x00FFFFFF) | mask;
	}

	doParse();

	return m_cells;
}


} // namespace modParser
