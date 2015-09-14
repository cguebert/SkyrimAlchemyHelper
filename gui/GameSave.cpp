#include "GameSave.h"

#include <saveParser/Save.h>
#include <QDir>
#include <QFileInfo>

#include "Config.h"
#include "Settings.h"

namespace
{

QString convert(const std::string& text)
{
	return QString::fromLatin1(text.c_str());
}

int getIngredientId(const saveParser::Save::Ingredient& ing)
{
	const auto& config = Config::main();
	auto pluginId = config.indexOfPlugin(convert(ing.mod));
	if (pluginId == -1)
		return -1;
	return config.indexOfIngredient(pluginId, ing.id);
}

}

GameSave::GameSave()
{
	const auto& settings = Settings::instance();
	m_maxValidIngredientCount = settings.maxValidIngredientCount;
	m_minValidNbIngredients = settings.minValidNbIngredients;
	m_minTotalIngredientsCount = settings.minTotalIngredientsCount;
}

void GameSave::load(QString fileName)
{
	clear();

	const auto& config = Config::main();
	const auto& plugins = config.plugins;
	const auto& ingredients = config.ingredients;
	const int nbIngredients = ingredients.size();

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
	save.setMaxValidIngredientCount(m_maxValidIngredientCount);
	save.setMinValidNbIngredients(m_minValidNbIngredients);
	save.setMinTotalIngredientsCount(m_minTotalIngredientsCount);
	m_isLoaded = save.parse(fileName.toStdString());
	if (!m_isLoaded)
		return;

	// Convert screenshot
	auto header = save.header();
	auto img = QImage(header.ssData.data(), header.ssWidth, header.ssHeight, QImage::Format_RGB888);
	m_screenshot.convertFromImage(img);

	// Copy the header
	m_header.playerLocation = convert(header.playerLocation);
	m_header.playerName = convert(header.playerName);
	m_header.playerLevel = header.playerLevel;
	m_header.saveNumber = header.saveNumber;

	// Convert known ingredients
	std::array<bool, 4> unknownIngredient = { false, false, false, false };
	m_knownIngredients.assign(nbIngredients, unknownIngredient);
	for (const auto& ing : save.knownIngredients())
	{
		auto ingId = getIngredientId(ing.first);
		if (ingId == -1)
			continue;

		m_knownIngredients[ingId] = ing.second;
	}

	// Convert all containers
	const auto& saveIngredients = save.listedIngredients();
	const auto saveContainers = save.containers();
	m_containers.reserve(saveContainers.size());
	m_ingredientsCount.assign(nbIngredients, 0);
	for (const auto& sc : saveContainers)
	{
		Container container;
		container.id = sc.id;
		for (const auto& ing : sc.inventory)
		{
			auto ingId = getIngredientId(saveIngredients[ing.first]);
			if (ingId == -1)
				continue;

			container.inventory.emplace_back(ingId, ing.second);
			m_ingredientsCount[ingId] += ing.second;
		}

		m_containers.push_back(std::move(container));
	}

	// Compute inventory (remove ingredients with a count of zero)
	m_inventory.clear();
	for (int i = 0; i < nbIngredients; ++i)
	{
		if (m_ingredientsCount[i])
			m_inventory.emplace_back(i, m_ingredientsCount[i]);
	}
}

void GameSave::loadSaveFromConfig()
{
	clear();

	auto& settings = Settings::instance();
	if (settings.savesFolder.isEmpty())
		return;

	if (!settings.loadMostRecentSave && !settings.selectedSavePath.isEmpty() && QFileInfo::exists(settings.selectedSavePath))
	{
		load(settings.selectedSavePath);
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
	m_containers.clear();
}

QFileInfoList GameSave::savesList()
{
	auto& settings = Settings::instance();
	if (settings.savesFolder.isEmpty())
		return QFileInfoList();

	QDir dir(settings.savesFolder);
	QStringList filters;
	filters << "*.ess";
	return dir.entryInfoList(filters, QDir::Files, QDir::Time);
}
