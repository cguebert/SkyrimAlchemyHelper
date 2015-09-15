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
		std::string baseName, cellName;
	};
	using Containers = std::vector<Container>;
	Containers findContainers(const std::vector<uint32_t>& ids);

protected:
	Containers m_containers;

	int m_currentContainerId = -1;
	uint32_t m_currentLocation = 0,
		m_subGroupsDepth = 0; // Depth after the location
	bool m_isInteriorCell = true; // False for worldspace cell
	const std::vector<std::string>& m_modsNames;
};

} // namespace modParser
