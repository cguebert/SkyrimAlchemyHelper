#pragma once

#include "Mod.h"
#include "Config.h"

namespace modParser
{

class ContainersParser : public Mod
{
public:
	using StringList = std::vector<std::string>;
	using ContainerType = std::pair<uint32_t, std::string>;
	using ContainerTypes = std::vector<ContainerType>;

	static void parse(const std::string& fileName, const std::string& language, const StringList& modsNames, Containers& containers, ContainerTypes& containerTypes);

protected:
	ContainersParser(const std::string& fileName, const std::string& language, const StringList& modsNames, Containers& containers, ContainerTypes& containerTypes);
	void findContainers();

	void registerContainersParsers();
	void registerTypesParsers();
	void getCellName();

	uint32_t toGlobal(uint32_t id); // From the masters list top the global list

	std::vector<uint8_t> m_mastersIds; // The ids of this mod's masters to their position in the complete mods list
	Containers& m_containers;
	const StringList& m_modsNames;
	ContainerTypes& m_containersTypes;

	struct CurrentCell
	{
		uint32_t id = 0, dataSize = 0, flags = 0;
		int subGroupsDepth = 0; // Depth after the location
		bool isInteriorCell = true; // False for worldspace cell
		std::vector<int> containersIndices;
		std::streamoff offset;
		std::string name;
	};
	CurrentCell m_currentCell;

	int m_currentId = -1;
	FieldParsers m_cellNameParser;
	std::vector<std::pair<uint32_t, int>> m_containersIds; // id - index; only for those that can be in this current mod
};

} // namespace modParser
