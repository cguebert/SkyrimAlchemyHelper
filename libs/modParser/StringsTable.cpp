#include <algorithm>
#include <sstream>
#include <iostream>

#include "StringsTable.h"
#include "BSAFile.h"

namespace modParser
{

using namespace std;

pair<string, string> getDirAndFile(const string& modFileName)
{
	pair<string, string> result;
	auto p = modFileName.find_last_of("/\\");
	if (p != string::npos)
	{
		result.first = modFileName.substr(0, p);
		++p;
	}
	else
		p = 0;

	auto e = modFileName.find_last_of(".");
	if (e != string::npos)
		result.second = modFileName.substr(p, e - p);
	else
		result.second = modFileName.substr(p);

	return result;
}

void StringsTable::load(const string& modFileName, const std::string& language)
{
	auto df = getDirAndFile(modFileName);
	string fileName = df.first + "/strings/" + df.second + "_" + language + ".strings";

	ifstream stream;
	stream.open(fileName, ios::binary | ios::in);
	if (!stream.is_open())
	{
		cerr << "Cannot open " << fileName << endl;
		BSAFile bsa;
		string modName = df.second;
		transform(modName.begin(), modName.end(), modName.begin(), ::tolower);
		if (modName == "skyrim")
			bsa.load(df.first + "/Skyrim - Interface.bsa");
		else
			bsa.load(modFileName);
		auto content = bsa.extract("strings/" + df.second + "_" + language + ".strings");

		if (content.empty())
		{
			cerr << "Cannot extract the string table from the BSA file" << endl;
			return;
		}
		else
		{
			istringstream ss(content);
			in.setStream(move(ss));
			cout << "Sucessfully extracted the string table from the BSA file" << endl;
		}
	}
	else
		in.setStream(move(stream));
	loadDirectory();
}

string StringsTable::readString()
{
	vector<char> buf;

	auto pos = in.tellg();
	int size = 64, prev = 0;
	buf.resize(size);
	in.stream().read(&buf.front(), size);

	while (find(buf.begin() + prev, buf.end(), 0) == buf.end())
	{
		prev = size;
		buf.resize(size * 2);
		in.stream().read(&buf.front() + prev, size);
		size *= 2;
	}

	auto end = find(buf.begin(), buf.end(), 0);
	in.seekg(pos + (end - buf.begin()));
	return string(buf.begin(), end);
}

string StringsTable::get(uint32_t id)
{
	auto it = lower_bound(m_offsets.begin(), m_offsets.end(), id, StringEntryComp());
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
	sort(m_offsets.begin(), m_offsets.end(), [](const StringEntry& lhs, const StringEntry& rhs){
		return lhs.first < rhs.first;
	});
}

} // namespace modParser
