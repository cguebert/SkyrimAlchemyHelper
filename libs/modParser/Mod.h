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
#include <functional>

#include "StringsTable.h"

namespace modParser
{

class Mod
{
public:
	static std::string getModName(const std::string& modFileName);

protected:
	Mod(const std::string& fileName, const std::string& language);
	void doParse();

	using ParseFieldFunc = std::function<void(uint16_t dataSize)>;
	struct FieldParser
	{
		FieldParser() = default;
		FieldParser(const std::string& type, ParseFieldFunc parseFunction)
			: type(type), parseFunction(parseFunction) {}

		std::string type;
		ParseFieldFunc parseFunction;
	};
	using FieldParsers = std::vector<FieldParser>;

	using BeginRecordFunc = std::function<bool(uint32_t id, uint32_t dataSize, uint32_t flags)>; // Return false to ignore this record (do not parse fields, endFunc not called)
	using EndRecordFunc = std::function<void(uint32_t id)>;
	struct RecordParser
	{
		RecordParser() = default;
		RecordParser(const std::string& type, const FieldParsers& fields, BeginRecordFunc beginFunction = nullptr, EndRecordFunc endFunction = nullptr)
			: type(type), fields(fields), beginFunction(beginFunction), endFunction(endFunction) {}

		std::string type;
		FieldParsers fields;
		BeginRecordFunc beginFunction;
		EndRecordFunc endFunction;
	};
	using RecordParsers = std::vector<RecordParser>;

	using BeginSubGroupFunc = std::function<void()>;
	using EndSubGroupFunc = std::function<void()>;
	struct GroupParser
	{
		GroupParser() = default;
		GroupParser(const std::string& type, const RecordParsers& records)
			: type(type), records(records) {}

		std::string type;
		RecordParsers records;
		BeginSubGroupFunc beginSubGroupFunction;
		EndSubGroupFunc endSubGroupFunction;
	};

	void parseGroup();
	void parseSubGroup(const GroupParser& group);
	void parseRecord(const RecordParser& recordParser);
	void ignoreRecord();
	void parseFields(const FieldParsers& fieldParsers, uint32_t dataSize, uint32_t flags = 0);
	void ignoreField();

	void parsePluginInformation();

	std::string getMaster(uint32_t id);
	std::string readLStringField(uint16_t dataSize);
	std::string readType();

	std::streamoff m_dataOffset = 0; // After the header
	std::vector<std::string> m_masters;
	bool m_useStringsTable = false, m_stringsTableLoaded = false;
	StringsTable m_stringsTable;
	parser::Parser in;
	std::string m_modFileName, m_modName, m_language;	
	std::vector<GroupParser> m_groupParsers;
};

} // namespace modParser
