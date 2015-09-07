#pragma once

#include <parser/Parser.h>

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
	using Ingredients = std::vector<Ingredient>;

	using KnownIngredient = std::pair<Ingredient, std::array<bool, 4>>;
	using KnownIngredients = std::vector<KnownIngredient>;
	const KnownIngredients& knownIngredients() const;

	void setPossibleIngredients(const Ingredients& ingredients);

	using InventoryItem = std::pair<Ingredient, int>;
	using Inventory = std::vector<InventoryItem>;
	const Inventory& inventory() const;
	
protected:
	void doParse();
	void computeIngredientsRefIDs();

	void parseHeader();
	void parseChangeForms();
	void parseFormIDArray();

	using RefID = std::array < uint8_t, 3 >;
	class ChangeForm
	{
	public:
		ChangeForm(Parser& parser);

		void ignore(); // Jump over the data
		void loadData(); // Decompress the data for use

		RefID refID;
		uint32_t formID = 0, changeFlags = 0;
		uint8_t formType= 0;
		std::vector<unsigned char> data;

	protected:
		Parser& in;
		uint32_t length1, length2;
	};

	void parseKnownIngredient(const ChangeForm& form);
	void parsePlayer(const ChangeForm& form);

	uint32_t getFormID(const RefID& refID);
	RefID getRefID(uint32_t formID);

	Parser in;
	uint32_t m_formIDArrayCountOffset, m_changeFormCount, m_changeFormsOffset;
	std::vector<uint32_t> m_formIDArray;
	std::vector<std::string> m_plugins;
	Header m_header;
	Ingredients m_possibleIngredients;
	std::vector<RefID> m_ingredientsRefID;
	KnownIngredients m_knownIngredients;
	Inventory m_inventory;
};

