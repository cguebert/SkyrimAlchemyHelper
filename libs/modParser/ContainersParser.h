/******************************************************************************
*                            Skyrim Alchemy Helper                            *
*******************************************************************************
*                                                                             *
* Copyright (C) 2015 Christophe Guebert                                       *
*                                                                             *
* This program is free software; you can redistribute it and/or modify        *
* it under the terms of the GNU General Public License as published by        *
* the Free Software Foundation; either version 2 of the License, or           *
* (at your option) any later version.                                         *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc.,     *
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                 *
*******************************************************************************
*                      Skyrim Alchemy Helper :: ModParser                     *
*                                                                             *
*                  Contact: christophe.guebert+SAH@gmail.com                  *
******************************************************************************/
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
	std::vector<std::pair<uint32_t, uint32_t>> m_containersIds; // id - index; only for those that can be in this current mod
};

} // namespace modParser
