#include <algorithm>
#include <sstream>
#include <iostream>

#include "StringsTable.h"

using namespace std;

string StringsTable::getStringsFileName(const std::string& modFileName)
{
	string dir;
	auto p = modFileName.find_last_of("/\\");
	if (p != string::npos)
	{
		dir = modFileName.substr(0, p);
		++p;
	}
	else
		p = 0;
	string fileName;
	auto e = modFileName.find_last_of(".");
	if (e != string::npos)
		fileName = modFileName.substr(p, e - p);
	else
		fileName = modFileName.substr(p);

	return dir + "/Strings/" + fileName + "_English.STRINGS";
}

void StringsTable::load(const std::string& modFileName)
{
	string fileName = getStringsFileName(modFileName);

	ifstream stream;
	stream.open(fileName, ios::binary | ios::in);
	if (!stream.is_open())
	{
		cout << "Cannot open " << fileName << endl;
		return;
	}

	in.setStream(std::move(stream));
	loadDirectory();
}

std::string StringsTable::readString()
{
	std::vector<char> buf;

	auto pos = in.tellg();
	int size = 64, prev = 0;
	buf.resize(size);
	in.stream().read(&buf.front(), size);

	while (std::find(buf.begin() + prev, buf.end(), 0) == buf.end())
	{
		prev = size;
		buf.resize(size*2);
		in.stream().read(&buf.front() + prev, size);
		size *= 2;
	}

	auto end = std::find(buf.begin(), buf.end(), 0);
	in.seekg(pos + (end - buf.begin()));
	return string(buf.begin(), end);
}

std::string StringsTable::get(uint32_t id)
{
	auto it = std::lower_bound(m_offsets.begin(), m_offsets.end(), id, StringEntryComp());
	if (it == m_offsets.end() || it->first != id)
		return "";

	in.seekg(it->second);
	return in.readZString();
}

void StringsTable::loadDirectory()
{
	uint32_t count, dataSize;
	in >> count >> dataSize;

	uint32_t start = 8 + 8 * count;
	for (uint32_t i = 0; i < count; ++i)
	{
		uint32_t id, offset;
		in >> id >> offset;
		m_offsets.emplace_back(id, start + offset);
	}

	// Sort the directory so we can use binary search
	std::sort(m_offsets.begin(), m_offsets.end(), [](const StringEntry& lhs, const StringEntry& rhs){
		return lhs.first < rhs.first;
	});
}