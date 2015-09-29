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

#include <parser/Parser.h>

namespace modParser
{

class StringsTable
{
public:
	void load(const std::string& modFileName, const std::string& language = "english");
	std::string get(uint32_t id);

protected:
	void loadDirectory();
	std::string readString();

	parser::Parser in;
	using StringEntry = std::pair < uint32_t, std::streamoff >;
	std::vector<StringEntry> m_offsets;

	struct StringEntryComp
	{
		bool operator() (const StringEntry& str, uint32_t id) { return str.first < id; }
		bool operator() (uint32_t id, const StringEntry& str) { return id < str.first; }
	};
};

} // namespace modParser
