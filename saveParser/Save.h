#pragma once

#include "Parser.h"

using Inventory = std::vector < std::pair< std::string, int> > ;

class Save
{
public:
	static void parse(const std::string& fileName);
	
protected:
	Save(const std::string& fileName);
	void doParse();

	void parseHeader();
	void parseChangeForms();
	void parseFormIDArray();

	using RefID = std::array < uint8_t, 3 > ;
	uint32_t getFormID(const RefID& refID);

	Parser in;
	uint32_t m_formIDArrayCountOffset, m_changeFormCount, m_changeFormsOffset,
		m_globalDataTable1Count, m_globalDataTable2Count, m_globalDataTable3Count,
		m_globalDataTable1Offset, m_globalDataTable2Offset, globalDataTable3Offset,
		m_unknownTable3Offset;
	std::vector<uint32_t> m_formIDArray;
	std::vector<std::string> m_plugins;
};

