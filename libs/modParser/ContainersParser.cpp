#include "ContainersParser.h"

#include <iostream>

namespace modParser
{

using namespace std;

void ContainersParser::parse(const std::string& fileName, const std::string& language, const StringList& modsNames, Containers& containers, ContainerTypes& containerTypes)
{
	ContainersParser parser(fileName, language, modsNames, containers, containerTypes);
	parser.findContainers();
}

ContainersParser::ContainersParser(const std::string& fileName, 
	const std::string& language, 
	const StringList& modsNames,
	Containers& containers,
	ContainerTypes& containerTypes)
	: Mod(fileName, language)
	, m_modsNames(modsNames)
	, m_containers(containers)
	, m_containersTypes(containerTypes)
{
	// We want to read the masters list now
	parsePluginInformation();

	// Compute the id of the masters in the whole mods list
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

	// Mod id to master id
	std::vector<int> modToMaster;
	modToMaster.assign(m_modsNames.size(), -1);
	for (int i = 0; i <= nbMasters; ++i)
	{
		if (m_mastersIds[i] != -1)
			modToMaster[m_mastersIds[i]] = i;
	}

	// Extract the containers that can be in this mod
	for (int i = 0, nbContainers = containers.size(); i < nbContainers; ++i)
	{
		auto& container = containers[i];
		int modId = container.id >> 24;
		int masterId = modToMaster[modId];
		if (masterId != -1)
		{
			uint32_t id2 = (container.id & 0x00FFFFFF) | (masterId << 24);
			m_containersIds.emplace_back(id2, i);
		}
	}
}

void ContainersParser::registerContainersParsers()
{
	m_groupParsers.clear();

// REFR
	RecordParser refrRecord;
	refrRecord.type = "REFR";
	refrRecord.beginFunction = [this](uint32_t id, uint32_t /*dataSize*/, uint32_t /*flags*/) {
		auto it = find_if(m_containersIds.begin(), m_containersIds.end(), [id](const std::pair<uint32_t, int>& p){
			return p.first == id;
		});
		if (it == m_containersIds.end() || !m_currentCell.id)
			return false;
		m_currentId = it->second;
		m_currentCell.containersIndices.push_back(m_currentId);
		return true;
	};

	refrRecord.fields.emplace_back("NAME", [this](uint16_t dataSize) {
		uint32_t id, type;
		in >> id;
		type = toGlobal(id);

		auto it = find_if(m_containersTypes.rbegin(), m_containersTypes.rend(), [type](const ContainerType& ct){
			return ct.first == type;
		});
		if (it != m_containersTypes.rend())
			m_containers[m_currentId].type = it->second;
		else
			cout << "Cannot find " << hex << uppercase << id << " " << type << dec << endl;
	});

// CELL
	RecordParser cellRecord;
	cellRecord.type = "CELL";
	cellRecord.beginFunction = [this](uint32_t id, uint32_t dataSize, uint32_t flags) {
		m_currentCell.id = id;
		m_currentCell.dataSize = dataSize;
		m_currentCell.flags = flags;
		m_currentCell.subGroupsDepth = 0;
		m_currentCell.isInteriorCell = true;
		m_currentCell.offset = in.tellg();
		return false;
	};

	GroupParser cellGroup("CELL",{ refrRecord, cellRecord });
	cellGroup.beginSubGroupFunction = [this]() {
		++m_currentCell.subGroupsDepth;
	};

	cellGroup.endSubGroupFunction = [this]() {
		--m_currentCell.subGroupsDepth;
		if (!m_currentCell.subGroupsDepth)
		{
			if (!m_currentCell.containersIndices.empty())
				getCellName();
			m_currentCell.id = 0;
		}
	};

	m_groupParsers.push_back(cellGroup);

// WRLD
	RecordParser worldRecord;
	worldRecord.type = "WRLD";
	worldRecord.beginFunction = [this](uint32_t id, uint32_t dataSize, uint32_t flags) {
		m_currentCell.id = id;
		m_currentCell.dataSize = dataSize;
		m_currentCell.flags = flags;
		m_currentCell.subGroupsDepth = 0;
		m_currentCell.isInteriorCell = false;
		m_currentCell.offset = in.tellg();
		return false;
	};

	GroupParser worldGroup("WRLD", { refrRecord, worldRecord });
	worldGroup.beginSubGroupFunction = [this]() {
		++m_currentCell.subGroupsDepth;
	};

	worldGroup.endSubGroupFunction = [this]() {
		--m_currentCell.subGroupsDepth;
		if (!m_currentCell.subGroupsDepth)
		{
			if (!m_currentCell.containersIndices.empty())
				getCellName();
			m_currentCell.id = 0;
		}
	};

	m_groupParsers.push_back(worldGroup);

// Cell name
	m_cellNameParser.emplace_back("FULL", [this](uint16_t dataSize) {
		m_currentCell.name = readLStringField(dataSize);
	});
}

void ContainersParser::registerTypesParsers()
{
// CONT
	RecordParser contRecord;
	contRecord.type = "CONT";
	contRecord.beginFunction = [this](uint32_t id, uint32_t dataSize, uint32_t flags) {
		m_containersTypes.emplace_back(toGlobal(id), "");
		return true;
	};

	contRecord.fields.emplace_back("FULL", [this](uint16_t dataSize) {
		m_containersTypes.back().second = readLStringField(dataSize);
	});

	GroupParser contGroup("CONT", { contRecord });
	m_groupParsers.push_back(contGroup);
}

void ContainersParser::findContainers()
{
	if (!m_containersIds.empty())
		registerContainersParsers();
	registerTypesParsers();

	doParse();
}

void ContainersParser::getCellName()
{
	auto start = in.tellg();
	in.seekg(m_currentCell.offset);
	m_currentCell.name.clear();
	parseFields(m_cellNameParser, m_currentCell.dataSize, m_currentCell.flags);
	in.seekg(start);

	if (!m_currentCell.name.empty())
	{
		for (auto id : m_currentCell.containersIndices)
		{
			auto& container = m_containers[id];
			container.cell = toGlobal(m_currentCell.id);
			container.location = m_currentCell.name;
			container.interior = m_currentCell.isInteriorCell;
		}
	}

	m_currentCell.containersIndices.clear();
}

uint32_t ContainersParser::toGlobal(uint32_t id)
{
	uint32_t masterId = id >> 24;
	if (masterId >= m_mastersIds.size())
		return (id & 0x00FFFFFF) | (m_mastersIds.back() << 24);
	else
		return (id & 0x00FFFFFF) | (m_mastersIds[masterId] << 24);
}


} // namespace modParser
