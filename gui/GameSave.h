#pragma once

#include <QString>
#include <QPixmap>

#include <array>
#include <vector>

class GameSave
{
public:
	static GameSave& instance();

	void loadSaveFromConfig();

	void load(QString fileName);
	bool isLoaded() const;

	QPixmap screenshot() const;

	struct Header
	{
		int saveNumber = 0, playerLevel = 0;
		QString playerName, playerLocation;
	};

	Header header() const;

	using KnownIngredient = std::pair<int, std::array<bool, 4>>; // Ingredient index in IngredientsList, bool for each magical effect
	using KnownIngredients = std::vector<KnownIngredient>;
	const KnownIngredients& knownIngredients() const;

	using InventoryItem = std::pair<int, int>; // Ingredient index in IngredientsList, number of ingredients
	using Inventory = std::vector<InventoryItem>;
	const Inventory& inventory() const;

protected:
	GameSave();

	bool m_isLoaded = false;
	QPixmap m_screenshot;
	Header m_header;
	KnownIngredients m_knownIngredients;
	Inventory m_inventory;
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