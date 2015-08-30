#pragma once

#include "Parser.h"

class StringsTable
{
public:
	void load(const std::string& modFileName);
	std::string get(uint32_t id);

protected:
	void loadDirectory();
	std::string readString();

	Parser in;
	using StringEntry = std::pair < uint32_t, std::streamoff > ;
	std::vector<StringEntry> m_offsets;

	struct StringEntryComp
	{
		bool operator() (const StringEntry& str, uint32_t id) { return str.first < id; }
		bool operator() (uint32_t id, const StringEntry& str) { return id < str.first; }
	};
};

