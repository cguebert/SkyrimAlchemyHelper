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

#include <parser/parser.h>

namespace modParser
{

class BSAFile
{
public:
	void load(const std::string& fileName);
	std::string extract(const std::string& fileName);

protected:
	std::string extractFile(int64_t dir, int64_t file);

	parser::Parser in;

	int32_t m_flags, m_filesNameLen;
	bool m_archiveCompressed;

	struct FolderRecord
	{
		int64_t hash;
		int32_t count, offset;
	};
	std::vector<FolderRecord> m_folders;

	struct FileRecord
	{
		int64_t hash;
		int32_t size, offset;
	};
};

} // namespace modParser
