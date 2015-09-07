#pragma once

#include <parser/Parser.h>

using Inventory = std::vector < std::pair< std::string, int> >;

class Save
{
public:
	bool parse(const std::string& fileName);

	struct Header
	{
		uint32_t saveNumber = 0;

		uint32_t ssWidth = 0, ssHeight = 0;
		std::vector<uint8_t> ssData;

		std::string playerName, playerLocation;
		uint32_t playerLevel = 0;
	};
	const Header& header() const;

	struct Ingredient
	{
		std::string mod;
		uint32_t id;
	};
	using KnownIngredient = std::pair<Ingredient, std::array<bool, 4>>;
	using KnownIngredients = std::vector<KnownIngredient>;
	const KnownIngredients& knownIngredients() const;
	
protected:
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
	Header m_header;
	KnownIngredients m_knownIngredients;
};

