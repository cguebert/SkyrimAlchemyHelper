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
#include "Mod.h"
#include "zlib/zlib.h"

#include <iostream>
#include <fstream>

namespace modParser
{

using namespace std;

string Mod::getModName(const string& modFileName)
{
	auto p = modFileName.find_last_of("/\\");
	if (p != string::npos)
		++p;
	else
		p = 0;
	return modFileName.substr(p);
}

Mod::Mod(const string& fileName, const std::string& language)
	: m_modFileName(fileName)
	, m_language(language)
{
	ifstream stream(fileName, ios::binary | ios::in);
	if (!stream.is_open())
	{
		cerr << "Cannot open " << fileName << endl;
		return;
	}

	in.setStream(move(stream));

	m_modName = getModName(fileName);
}

void Mod::doParse()
{
	if (!m_dataOffset)
		parsePluginInformation();
	else
	{
		in.stream().clear();
		in.seekg(m_dataOffset);
	}
	
	// Top level records
	while (!in.eof())
	{
		string type = readType();
		if (in.stream().eof())
			return;
		else if (type == "GRUP")
			parseGroup();
		else
			ignoreRecord();
	}
}

void Mod::parseGroup()
{
	auto start = in.tellg() - 4;
	uint32_t groupSize;
	in >> groupSize;
	string groupType = readType();
	in.jump(12);

	bool parsedGroup = false;
	for (auto& group : m_groupParsers)
	{
		if (group.type == groupType)
		{
			while (in.tellg() - start < groupSize)
			{
				string recordType = readType();
				if (recordType == "GRUP")
					parseSubGroup(group);
				else
				{
					bool parsed = false;
					for (auto& record : group.records)
					{
						if (record.type == recordType)
						{
							parseRecord(record);
							parsed = true;
							break;
						}
					}
					if (!parsed)
						ignoreRecord();
				}
			}

			parsedGroup = true;
			break;
		}
	}

	if (!parsedGroup)
		in.seekg(start + groupSize);
}

void Mod::parseSubGroup(const GroupParser& group)
{
	if (group.beginSubGroupFunction)
		group.beginSubGroupFunction();

	auto start = in.tellg() - 4;
	uint32_t groupSize;
	in >> groupSize;
	string groupType = readType();
	in.jump(12);

	while (in.tellg() - start < groupSize)
	{
		string recordType = readType();
		if (recordType == "GRUP")
			parseSubGroup(group);
		else
		{
			bool parsed = false;
			for (auto& record : group.records)
			{
				if (record.type == recordType)
				{
					parseRecord(record);
					parsed = true;
					break;
				}
			}
			if (!parsed)
				ignoreRecord();
		}
	}

	if (group.endSubGroupFunction)
		group.endSubGroupFunction();
}

void Mod::parseRecord(const RecordParser& recordParser)
{
	uint32_t dataSize, flags, id;
	in >> dataSize >> flags >> id;
	in.jump(8);

	if (recordParser.beginFunction)
	{
		if (!recordParser.beginFunction(id, dataSize, flags))
		{
			in.jump(dataSize);
			return;
		}
	}

	if (recordParser.fields.empty())
		in.jump(dataSize);
	else
		parseFields(recordParser.fields, dataSize, flags);

	if (recordParser.endFunction)
		recordParser.endFunction(id);
}

void Mod::ignoreRecord()
{
	uint32_t dataSize;
	in >> dataSize;
	in.jump(dataSize + 16);
}

void Mod::parseFields(const FieldParsers& fieldParsers, uint32_t dataSize, uint32_t flags)
{
	bool compressed = false;
	istringstream ss;
	istream* oldPtr;
	if (flags & 0x40000) // Compressed
	{
		compressed = true;
		uint32_t decompSize;
		in >> decompSize;
		std::vector<unsigned char> compressedData;
		compressedData.resize(dataSize - 4);
		in >> compressedData;

		string decompressedData;
		decompressedData.resize(decompSize);

		uLongf decSize = decompSize;
		uncompress(reinterpret_cast<Bytef*>(&decompressedData[0]), &decSize, compressedData.data(), dataSize - 4);
		ss.str(decompressedData);

		oldPtr = in.streamPtr();
		in.streamPtr() = &ss;
		dataSize = decSize;
	}

	auto start = in.tellg();
	while (in.tellg() - start < dataSize)
	{
		string type = readType();
		uint16_t dataSize;
		in >> dataSize;
		auto startField = in.tellg(); // TEMP DEBUG
		bool parsed = false;
		for (const auto& field : fieldParsers)
		{
			if (field.type == type)
			{
				if (field.parseFunction)
				{
					field.parseFunction(dataSize);
					parsed = true;
				}
				break;
			}
		}

		if (!parsed)
			in.jump(dataSize);

		if (in.tellg() != startField + dataSize) // TEMP DEBUG
		{
			cerr << "Error in parsing field " << type << endl;
			exit(1);
		}
	}

	if (compressed)
		in.streamPtr() = oldPtr;
}

void Mod::parsePluginInformation()
{
	string type = readType();
	if (type != "TES4")
	{
		cerr << "Error: No TES4 record at the start of the file" << endl;
		return;
	}

	uint32_t dataSize, flags;
	in >> dataSize >> flags;
	in.jump(12);

	m_useStringsTable = (flags & 0x80) != 0;

	FieldParsers fields;
	fields.emplace_back("MAST", [this](uint16_t dataSize){
		string name;
		name.resize(dataSize - 1); // Don't read null character in the string
		in.stream().read(&name[0], dataSize - 1);
		in.jump(1);

		m_masters.push_back(getModName(name));
	});
	parseFields(fields, dataSize);

	m_dataOffset = in.tellg();
}

std::string Mod::getMaster(uint32_t id)
{
	uint8_t modId = id >> 24;

	unsigned int nbMasters = m_masters.size();
	if (modId == nbMasters) // Introduced by this mod
		return m_modName;
	else if (modId < nbMasters)
		return m_masters[modId];
	else
	{
		cerr << "Error: invalid modId " << hex << (int)modId << endl;
		return "";
	}
}

string Mod::readLStringField(uint16_t dataSize)
{
	if (m_useStringsTable)
	{
		uint32_t id;
		in >> id;
		if (!m_stringsTableLoaded)
		{
			m_stringsTable.load(m_modFileName, m_language);
			m_stringsTableLoaded = true;
		}
		return m_stringsTable.get(id);
	}
	else
	{
		string text;
		text.resize(dataSize - 1); // Don't read null character in the string
		in.stream().read(&text[0], dataSize - 1);
		in.jump(1);
		return text;
	}
}

std::string Mod::readType()
{
	return in.readString(4);
}

} // namespace modParser
