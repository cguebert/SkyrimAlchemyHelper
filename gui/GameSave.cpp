#include "GameSave.h"

#include <saveParser/Save.h>
#include <QDir>
#include <QFileInfo>

#include "Config.h"
#include "IngredientsList.h"
#include "PluginsList.h"

GameSave& GameSave::instance()
{
	static GameSave save;
	return save;
}

GameSave::GameSave()
{
	loadSaveFromConfig();
}

void GameSave::load(QString fileName)
{
	m_screenshot = QPixmap();
	m_knownIngredients.clear();

	Save save;
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
	const auto& plugins = PluginsList::instance();
	const auto& ingredients = IngredientsList::instance();
	for (auto ing : save.knownIngredients())
	{
		auto pluginId = plugins.find(ing.first.mod.c_str());
		if (pluginId == -1)
			continue;
		auto ingId = ingredients.find(pluginId, ing.first.id);
		if (ingId == -1)
			continue;

		m_knownIngredients.emplace_back(ingId, ing.second);
	}
}

void GameSave::loadSaveFromConfig()
{
	auto& config = Config::instance();
	if (config.savesFolder.isEmpty())
		return;

	if (!config.loadMostRecentSave && !config.selectedSaveName.isEmpty() && QFileInfo::exists(config.selectedSaveName))
	{
		load(config.selectedSaveName);
		return;
	}

	QDir dir(config.savesFolder);
	QStringList filters;
	filters << "*.ess";
	auto saves = dir.entryInfoList(filters, QDir::Files, QDir::Time);
	if (saves.empty())
		return;

	load(saves.first().absoluteFilePath());
}