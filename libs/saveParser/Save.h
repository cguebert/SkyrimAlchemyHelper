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
*                     Skyrim Alchemy Helper :: SaveParser                     *
*                                                                             *
*                  Contact: christophe.guebert+SAH@gmail.com                  *
******************************************************************************/
#pragma once

#include <parser/Parser.h>

namespace saveParser
{
class Save
{
public:
	bool parse(const std::string& fileName);

	enum class CompressionMethod
	{
		none = 0,
		zlib = 1,
		lz4 = 2
	};

	struct Header
	{
		uint32_t saveVersion = 0;
		bool skyrimSE = false;
		CompressionMethod compressionMethod = CompressionMethod::none;

		uint32_t saveNumber = 0;

		uint32_t ssWidth = 0, ssHeight = 0;
		std::vector<uint8_t> ssData;
		bool ssWithAlpha = false; // If true, the screen shot is saved using 4 bytes per pixel

		std::string playerName, playerLocation;
		uint32_t playerLevel = 0;

		uint32_t worldSpace1 = 0, worldSpace2 = 0;
		int32_t coorX = 0, coorY = 0;

		uint8_t formVersion = 0;
	};
	const Header& header() const;

	struct Ingredient
	{
		std::string mod;
		uint32_t id;
	};
	using Ingredients = std::vector<Ingredient>;

	using StringsList = std::vector<std::string>;
	const StringsList& masters() const;

	using KnownIngredient = std::pair<Ingredient, std::array<bool, 4>>; // Index in listedIngredients, bool for each effect of the ingredient
	using KnownIngredients = std::vector<KnownIngredient>;
	const KnownIngredients& knownIngredients() const;

	void setPossibleIngredients(const Ingredients& ingredients); // Set all ingredients to look for in the save
	const Ingredients& listedIngredients() const;                // Subset of the possible ingredients that are listed in this save

	void setMaxValidIngredientCount(int count);  // Everything higher than this is considered invalid (if 0, accept everything)
	void setMinValidNbIngredients(int nb);       // Ignore containers (and the player inventory) if there is less than nb different ingredients (if 0, accept everything)
	void setMinTotalIngredientsCount(int count); // Ignore containers if there are less than total count ingredients of all types
	void setPlayerOnly(bool playerOnly);         // Do not parse containers

	using InventoryItem = std::pair<int, int>; // Index in listedIngredients, count
	using Inventory = std::vector<InventoryItem>;
	struct Container
	{
		uint32_t id;
		Inventory inventory;
	};
	using Containers = std::vector<Container>;
	const Containers& containers() const;

protected:
	void doParse();
	void computeIngredientsRefIDs();

	void parseHeader();
	void parseChangeForms();
	void parseFormIDArray();

	using RefID = std::array<uint8_t, 3>;
	using RefIDs = std::vector<RefID>;

	class ChangeForm
	{
	public:
		ChangeForm(parser::Parser& parser);

		void ignore();   // Jump over the data
		void loadData(); // Decompress the data for use

		RefID refID;
		uint32_t formID = 0, changeFlags = 0;
		uint8_t formType = 0;
		std::vector<unsigned char> data;
		bool ok = false;

	protected:
		parser::Parser& in;
		uint32_t length1, length2;
	};

	// Optimization of the ingredients search in containers
	class SearchHelper
	{
	public:
		using Buffer = std::vector<unsigned char>;
		using SearchResult = std::pair<bool, RefID>;

		void setup(const RefIDs& refIDs);
		int search(const Buffer& buffer, int& pos); // Returns the index in m_ingredientsRefID, or -1 if not found; pos is set to the location where the RefID is found

	protected:
		uint16_t levels[3][256];
		std::vector<uint16_t> data; // Storing something like a tree in an array
	};

	void getPlayerLocation();
	void parseKnownIngredient(const ChangeForm& form);
	void parsePlayer(const ChangeForm& form);
	void parseContainer(const ChangeForm& form);
	Inventory searchForIngredients(const ChangeForm& form);

	uint32_t getFormID(const RefID& refID);
	RefID getRefID(uint32_t formID);

	parser::Parser in;
	uint32_t m_formIDArrayCountOffset,
		m_globalDataTable1Offset, m_globalDataTable1Count,
		m_changeFormsOffset, m_changeFormCount;
	std::vector<uint32_t> m_formIDArray;
	StringsList m_plugins;
	Header m_header;
	Ingredients m_possibleIngredients, m_listedIngredients;
	RefIDs m_ingredientsRefID;
	KnownIngredients m_knownIngredients;
	Containers m_containers;
	SearchHelper m_searchHelper;
	int m_maxValidIngredientCount = 0, m_minValidNbIngredients = 0, m_minTotalIngredientsCount = 0;
	bool m_playerOnly = false; // If true, do not parse containers
};

//****************************************************************************//

inline const Save::Header& Save::header() const
{
	return m_header;
}

inline const Save::KnownIngredients& Save::knownIngredients() const
{
	return m_knownIngredients;
}

inline void Save::setPossibleIngredients(const Ingredients& ingredients)
{
	m_possibleIngredients = ingredients;
}

inline const Save::Ingredients& Save::listedIngredients() const
{
	return m_listedIngredients;
}

inline const Save::Containers& Save::containers() const
{
	return m_containers;
}

inline void Save::setMaxValidIngredientCount(int count)
{
	m_maxValidIngredientCount = count;
}

inline void Save::setMinValidNbIngredients(int nb)
{
	m_minValidNbIngredients = nb;
}

inline void Save::setMinTotalIngredientsCount(int count)
{
	m_minTotalIngredientsCount = count;
}

inline void Save::setPlayerOnly(bool playerOnly)
{
	m_playerOnly = playerOnly;
}

inline const Save::StringsList& Save::masters() const
{
	return m_plugins;
}

} // namespace saveParser
