#include "Mod.h"

#include <iostream>
#include <fstream>

namespace modParser
{

using namespace std;

string getModName(const string& modFileName)
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
	// Top level records
	string type = readType();
	if (type == "TES4")
		parsePluginInformation();
	else
		cerr << "Error: No TES4 record at the start of the file" << endl;

	while (!in.eof())
	{
		type = readType();
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

			parsedGroup = true;
			break;
		}
	}

	if (!parsedGroup)
		in.seekg(start + groupSize);
}

void Mod::parseRecord(const RecordParser& recordParser)
{
	uint32_t dataSize, flags, id;
	in >> dataSize >> flags >> id;
	in.jump(8);

	if (recordParser.beginFunction)
		recordParser.beginFunction(id, dataSize, flags);

	parseFields(recordParser.fields, dataSize);

	if (recordParser.endFunction)
		recordParser.endFunction(id);
}

void Mod::ignoreRecord()
{
	uint32_t dataSize;
	in >> dataSize;
	in.jump(dataSize + 16);
}

void Mod::parseFields(const FieldParsers& fieldParsers, uint32_t dataSize)
{
	auto start = in.tellg();
	while (in.tellg() - start < dataSize)
	{
		string type = readType();
		bool parsed = false;
		for (const auto& field : fieldParsers)
		{
			if (field.type == type)
			{
				if (field.parseFunction)
				{
					field.parseFunction();
					parsed = true;
				}
				break;
			}
		}

		if (!parsed)
			ignoreField();
	}
}

void Mod::ignoreField()
{
	uint16_t dataSize;
	in >> dataSize;
	in.jump(dataSize);
}

void Mod::parsePluginInformation()
{
	uint32_t dataSize, flags;
	in >> dataSize >> flags;
	in.jump(12);

	m_useStringsTable = (flags & 0x80) != 0;
	if (m_useStringsTable)
		m_stringsTable.load(m_modFileName, m_language);

	FieldParsers fields;
	fields.emplace_back("MAST", [this](){
		uint16_t dataSize;
		in >> dataSize;

		string name;
		name.resize(dataSize - 1); // Don't read null character in the string
		in.stream().read(&name[0], dataSize - 1);
		in.jump(1);

		m_masters.push_back(getModName(name));
	});
	parseFields(fields, dataSize);
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

string Mod::readLStringField()
{
	uint16_t dataSize;
	in >> dataSize;

	if (m_useStringsTable)
	{
		uint32_t id;
		in >> id;
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
