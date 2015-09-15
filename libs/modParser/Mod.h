#pragma once

#include <parser/Parser.h>
#include <functional>

#include "StringsTable.h"

namespace modParser
{

class Mod
{
protected:
	Mod(const std::string& fileName, const std::string& language);
	void doParse();

	using ParseFieldFunc = std::function<void()>;
	struct FieldParser
	{
		FieldParser() = default;
		FieldParser(const std::string& type, ParseFieldFunc parseFunction)
			: type(type), parseFunction(parseFunction) {}

		std::string type;
		ParseFieldFunc parseFunction;
	};
	using FieldParsers = std::vector<FieldParser>;

	using BeginRecordFunc = std::function<void(uint32_t id, uint32_t dataSize, uint32_t flags)>;
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

	struct GroupParser
	{
		GroupParser() = default;
		GroupParser(const std::string& type, const RecordParsers& records)
			: type(type), records(records) {}

		std::string type;
		RecordParsers records;
	};

	void parseGroup();
	void parseRecord(const RecordParser& recordParser);
	void ignoreRecord();
	void parseFields(const FieldParsers& fieldParsers, uint32_t dataSize);
	void ignoreField();

	void parsePluginInformation();

	std::string getMaster(uint32_t id);
	std::string readLStringField();
	std::string readType();

	std::vector<std::string> m_masters;
	bool m_useStringsTable = false;
	StringsTable m_stringsTable;
	parser::Parser in;
	std::string m_modFileName, m_modName, m_language;	
	std::vector<GroupParser> m_groupParsers;
};

} // namespace modParser
