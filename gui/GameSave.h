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
*                        Skyrim Alchemy Helper :: Gui                         *
*                                                                             *
*                  Contact: christophe.guebert+SAH@gmail.com                  *
******************************************************************************/
#pragma once

#include <QString>
#include <QPixmap>
#include <QFileInfo>

#include <array>
#include <vector>

class GameSave
{
public:
	static GameSave& instance() // Not a singleton, just the one everyone use
	{ static GameSave save; return save; }

	GameSave();

	QFileInfoList savesList();
	void loadSaveFromConfig();
	void load(QString fileName);
	bool isLoaded() const;

	void setMaxValidIngredientCount(int count); // Everything higher than this is considered invalid (if 0, accept everything)
	void setMinValidNbIngredients(int nb); // Ignore containers (and the player inventory) if there is less than nb different ingredients (if 0, accept everything)
	void setMinTotalIngredientsCount(int count); // Ignore containers if there are less than total count ingredients of all types
	void setPlayerOnly(bool playerOnly); // If true, ignore all containers and parse only the player (default: false)
	void setFilterSameCellAsPlayer(bool sameCell); // If true, ignore containers that are not in the same cell as the player
	void setInteriorCellsOnly(bool interior); // IF true, ignore containers that are out in the world
	
	void filterContainers(); // Apply sameCellAsPlayer
	void computeIngredientsCount();

	QPixmap screenshot() const;

	struct Header
	{
		int saveNumber = 0, playerLevel = 0;
		uint32_t locationId = 0;
		QString playerName, playerLocation;
	};

	Header header() const;

	using KnownIngredients = std::vector<std::array<bool, 4>>;
	const KnownIngredients& knownIngredients() const; // Bool for each magical effect

	using InventoryItem = std::pair<size_t, size_t>; // Ingredient index in IngredientsList, number of ingredients
	using Inventory = std::vector<InventoryItem>;
	const Inventory& inventory() const; // Total of all containers

	struct Container
	{
		uint32_t id;
		Inventory inventory;
	};
	using Containers = std::vector<Container>;
	const Containers& containers() const; // Player inventory, and all containers depending on the parse parameters

	std::vector<bool>& containersState();

	using IngredientsCount = std::vector<size_t>;
	const IngredientsCount& ingredientsCount() const; // Same as inventory, but more direct and with the 0-count ingredients

protected:
	void clear();

	bool m_isLoaded = false;
	QPixmap m_screenshot;
	Header m_header;
	KnownIngredients m_knownIngredients;
	Inventory m_inventory;
	Containers m_containers;
	std::vector<size_t> m_ingredientsCount;
	int m_maxValidIngredientCount = 1000, m_minValidNbIngredients = 5, m_minTotalIngredientsCount = 50;
	bool m_playerOnly = false, m_sameCellAsPlayer = false, m_interiorCellsOnly = false;
	std::vector<bool> m_containersState;
};

//****************************************************************************//

inline bool GameSave::isLoaded() const
{ return m_isLoaded; }

inline QPixmap GameSave::screenshot() const
{ return m_screenshot; }

inline GameSave::Header GameSave::header() const
{ return m_header; }

inline const GameSave::KnownIngredients& GameSave::knownIngredients() const
{ return m_knownIngredients; }

inline const GameSave::Inventory& GameSave::inventory() const
{ return m_inventory; }

inline const std::vector<size_t>& GameSave::ingredientsCount() const
{ return m_ingredientsCount; }

inline void GameSave::setMaxValidIngredientCount(int count)
{ m_maxValidIngredientCount = count; }

inline void GameSave::setMinValidNbIngredients(int nb)
{ m_minValidNbIngredients = nb; }

inline void GameSave::setMinTotalIngredientsCount(int count)
{ m_minTotalIngredientsCount = count; }

inline void GameSave::setPlayerOnly(bool playerOnly)
{ m_playerOnly = playerOnly; }

inline void GameSave::setFilterSameCellAsPlayer(bool sameCell)
{ m_sameCellAsPlayer = sameCell; }

inline void GameSave::setInteriorCellsOnly(bool interior)
{ m_interiorCellsOnly = interior; }

inline const GameSave::Containers& GameSave::containers() const
{ return m_containers; }

inline std::vector<bool>& GameSave::containersState()
{ return m_containersState; }
