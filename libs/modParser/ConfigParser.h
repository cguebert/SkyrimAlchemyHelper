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

class ConfigParser : public Mod
{
public:
	static void parse(const std::string& fileName, const std::string& language, Config& config);
	
protected:
	ConfigParser(const std::string& fileName, const std::string& language, Config& config);

	bool setIngredient(const Ingredient& ingredient); // Return true if adding, false if modifying existing
	void updateMagicalEffects();

	bool m_parsedMGEF = false;
	size_t m_nbIngrAdded = 0, m_nbIngrModified = 0,
		   m_nbEffAdded = 0, m_nbEffModified = 0;

	Ingredient m_currentIngredient;
	MagicalEffect m_currentMagicalEffect;
	Config& m_config;

	struct MGEFEntry
	{
		MGEFEntry() = default;
		MGEFEntry(uint32_t id, uint32_t dataSize, std::streamoff offset)
			: id(id), dataSize(dataSize), offset(offset) {}

		uint32_t id, dataSize;
		std::streamoff offset;
	};
	std::vector<MGEFEntry> m_magicalEffectsOffsets;

	struct MGEFEntryComp
	{
		uint32_t idOf(const MGEFEntry& t) { return t.id; }
		uint32_t idOf(const uint32_t& t) { return t; }

		template <class T, class U>
		bool operator() (const T& lhs, const U& rhs) { return idOf(lhs) < idOf(rhs); }
	};
};

} // namespace modParser
