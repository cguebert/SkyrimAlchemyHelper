#include "ContainersParser.h"

#include <iostream>

namespace modParser
{

using namespace std;

ContainersParser::ContainersParser(const std::string& fileName, const std::string& language, const std::vector<std::string>& modsNames)
	: Mod(fileName, language)
	, m_modsNames(modsNames)
{
// REFR
	RecordParser refrRecord;
	refrRecord.type = "REFR";
	refrRecord.beginFunction = [this](uint32_t id, uint32_t /*dataSize*/, uint32_t /*flags*/) {
		auto it = find_if(m_containers.begin(), m_containers.end(), [id](const Container& c){
			return c.id2 == id;
		});
		if (it == m_containers.end() || !m_currentLocation)
			return false;
		m_currentContainerId = it - m_containers.begin();
		it->cell = m_currentLocation;
		it->interior = m_isInteriorCell;
		return true;
	};

	refrRecord.endFunction = [this](uint32_t id) {
		const auto& container = m_containers[m_currentContainerId];
	//	cout << hex << uppercase << container.id << " " << container.base << " " << container.cell << dec << endl;
	};

	refrRecord.fields.emplace_back("NAME", [this](uint16_t dataSize) {
		in >> m_containers[m_currentContainerId].base;
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

	GroupParser cellGroup = { "CELL", RecordParsers({ refrRecord, cellRecord }) };
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

	GroupParser worldGroup = { "WRLD", RecordParsers({ refrRecord, worldRecord }) };
	worldGroup.beginSubGroupFunction = [this]() {
		++m_subGroupsDepth;
	};

	worldGroup.endSubGroupFunction = [this]() {
		if (!m_subGroupsDepth)
			m_currentLocation = 0;
		--m_subGroupsDepth;
	};

	m_groupParsers.push_back(worldGroup);

	// We want to read the masters list now
	parsePluginInformation();
}

ContainersParser::Containers ContainersParser::findContainers(const std::vector<uint32_t>& ids)
{
	const int nb = ids.size();
	m_containers.clear();
	m_containers.resize(nb);

	int nbMasters = m_masters.size();
	uint32_t mask = nbMasters << 24;
	for (int i = 0; i < nb; ++i)
	{
		auto& container = m_containers[i];
		const auto& id = ids[i];

		container.id = id;
		container.id2 = (id & 0x00FFFFFF) | mask;
	}

	doParse();

	std::vector<uint8_t> mastersIds; // The ids of this mod's masters to their position in the complete mods list
	mastersIds.assign(nbMasters + 1, -1);
	for (int i = 0; i < nbMasters; ++i)
	{
		auto it = find(m_modsNames.begin(), m_modsNames.end(), m_masters[i]);
		if (it != m_modsNames.end())
			mastersIds[i] = it - m_modsNames.begin();
	}

	auto it = find(m_modsNames.begin(), m_modsNames.end(), m_modName);
	if (it != m_modsNames.end())
		mastersIds[nbMasters] = it - m_modsNames.begin();

	// Convert FormIDs from this mod to the complete mods list
	for (auto& container : m_containers)
	{
		int8_t masterId = container.base >> 24;
		int32_t modMask = mastersIds[masterId] << 24;
		container.base = (container.base & 0x00FFFFFF) | modMask;

		masterId = container.cell >> 24;
		modMask = mastersIds[masterId] << 24;
		container.cell = (container.cell & 0x00FFFFFF) | modMask;
	}

	return m_containers;
}


} // namespace modParser
