#include "GameSave.h"

#include <saveParser/Save.h>
#include <QDir>
#include <QFileInfo>

#include "IngredientsList.h"
#include "PluginsList.h"
#include "Settings.h"

int getIngredientId(const saveParser::Save::Ingredient& ing)
{
	auto pluginId = PluginsList::instance().find(ing.mod.c_str());
	if (pluginId == -1)
		return -1;
	return IngredientsList::instance().find(pluginId, ing.id);
}

void GameSave::load(QString fileName)
{
	clear();

	const auto& plugins = PluginsList::instance().plugins();
	const auto& ingredients = IngredientsList::instance().ingredients();

	saveParser::Save::Ingredients possibleIngredients;
	for (const auto& ing : ingredients)
	{
		saveParser::Save::Ingredient pIng;
		pIng.mod = plugins[ing.pluginId].name.toStdString();
		pIng.id = ing.code;
		possibleIngredients.push_back(pIng);
	}

	saveParser::Save save;
	save.setPossibleIngredients(possibleIngredients);
	m_isLoaded = save.parse(fileName.toStdString());
	if (!m_isLoaded)
		return;

	// Convert screenshot
	auto header = save.header();
	auto img = QImage(header.ssData.data(), header.ssWidth, header.ssHeight, QImage::Format_RGB888);
	m_screenshot.convertFromImage(img);

	// Copy the header
	m_header.playerLocation = header.playerLocation.c_str();
	m_header.playerName = header.playerName.c_str();
	m_header.playerLevel = header.playerLevel;
	m_header.saveNumber = header.saveNumber;

	// Convert known ingredients
	for (const auto& ing : save.knownIngredients())
	{
		auto ingId = getIngredientId(ing.first);
		if (ingId == -1)
			continue;

		m_knownIngredients.emplace_back(ingId, ing.second);
	}

	// Convert inventory
	m_ingredientsCount.resize(ingredients.size());
	const auto& saveIngredients = save.listedIngredients();
	for (const auto& ing : save.inventory())
	{
		auto ingId = getIngredientId(saveIngredients[ing.first]);
		if (ingId == -1)
			continue;

		m_inventory.emplace_back(ingId, ing.second);
		m_ingredientsCount[ingId] = ing.second;
	}
}

void GameSave::loadSaveFromConfig()
{
	clear();

	auto& settings = Settings::instance();
	if (settings.savesFolder.isEmpty())
		return;

	if (!settings.loadMostRecentSave && !settings.selectedSaveName.isEmpty() && QFileInfo::exists(settings.selectedSaveName))
	{
		load(settings.selectedSaveName);
		return;
	}

	QDir dir(settings.savesFolder);
	QStringList filters;
	filters << "*.ess";
	auto saves = dir.entryInfoList(filters, QDir::Files, QDir::Time);
	if (saves.empty())
		return;

	load(saves.first().absoluteFilePath());
}

void GameSave::clear()
{
	m_isLoaded = false;
	m_screenshot = QPixmap();
	m_header = Header();
	m_knownIngredients.clear();
	m_inventory.clear();
	m_ingredientsCount.clear();
}
