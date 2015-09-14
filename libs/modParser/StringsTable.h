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
