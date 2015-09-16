#pragma once

#include "Mod.h"

namespace modParser
{

class ContainersParser : public Mod
{
public:
	ContainersParser(const std::string& fileName, const std::string& language, const std::vector<std::string>& modsNames);

	struct Container
	{
		uint32_t id = 0, id2 = 0, base = 0, cell = 0;
		bool interior = true;
	};
	using Containers = std::vector<Container>;
	Containers findContainers(const std::vector<uint32_t>& ids);

	// Cell or container type
	struct NameStruct
	{
		uint32_t id = 0, id2 = 0;
		std::string name;
	};
	using NameStructs = std::vector<NameStruct>;
	std::pair<NameStructs, NameStructs> getNames(const std::vector<uint32_t>& cellIds, const std::vector<uint32_t>& baseIds);

protected:
	void registerContainersParsers();
	void registerNamesParsers();

	std::vector<uint8_t> m_mastersIds; // The ids of this mod's masters to their position in the complete mods list
	Containers m_containers;
	NameStructs m_cells, m_containerTypes;

	int m_currentId = -1;
	uint32_t m_currentLocation = 0,
		m_subGroupsDepth = 0; // Depth after the location
	bool m_isInteriorCell = true; // False for worldspace cell
	const std::vector<std::string>& m_modsNames;
};

} // namespace modParser
