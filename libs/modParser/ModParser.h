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
#include "Config.h"

namespace modParser
{

class ModParser
{
public:
	using StringsList = std::vector<std::string>;
	void setModsList(const StringsList& list); // Set all mods that will be parsed
	void setLanguage(const std::string& language);

	Config parseConfig(); // Parse all mods and look for alchemy ingredients and their effects
	void exportConfig(const Config& config); // Useful for debug purposes

	using IdsList = std::vector<uint32_t>;
	Containers getContainersInfo(const IdsList& ids);

protected:
	StringsList m_modsList;
	std::string m_language = "english";
};

} // namespace modParser
