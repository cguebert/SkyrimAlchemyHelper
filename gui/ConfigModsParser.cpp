#include <QtWidgets>

#include <fstream>
#include <sstream>

#include "ConfigModsParser.h"

namespace
{

QString convert(const std::string& text) 
{ 
	return QString::fromLatin1(text.c_str()); 
}

std::string loadFile(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
		return "";
	std::stringstream stream;
	stream << file.rdbuf();
	file.close();
	return stream.str();
}

}

ConfigModsParser::ConfigModsParser(bool useModOrganizer,
	QString dataFolder,
	QString pluginsListPath,
	QString modOrganizerPath,
	QString language)
	: m_useModOrganizer(useModOrganizer)
	, m_dataFolder(dataFolder)
	, m_pluginsListPath(pluginsListPath)
	, m_modOrganizerPath(modOrganizerPath)
	, m_language(language)
{
}

ConfigModsParser::Result ConfigModsParser::parse()
{
	std::vector<std::string> modsPathList;
	if (!getModsPaths(modsPathList))
		return Result::Error_ModOrganizer;

	modParser::ModParser modParser;
	modParser.setModsList(modsPathList);
	modParser.setLanguage(m_language.toStdString());
	auto config = modParser.parseConfig();

	if (m_config.ingredientsList.empty())
		return Result::Error_ModsParsing;

	return Result::Success;
}

bool ConfigModsParser::getModsPaths(std::vector<std::string>& modsPathList)
{
	// Add "Skyrim.esm" if not using Mod Organizer
	if (!m_useModOrganizer)
	{
		std::string inList = loadFile(m_pluginsListPath.toStdString());
		std::transform(inList.begin(), inList.end(), inList.begin(), ::tolower);
		if (inList.find("skyrim.esm") == std::string::npos)
			modsPathList.emplace_back("Skyrim.esm");
	}

	// Loading the "config.plugins.txt" file
	std::ifstream inFile(m_pluginsListPath.toStdString());
	std::string modName;
	while (std::getline(inFile, modName))
	{
		if (modName[0] == '#')
			continue;

		modsPathList.emplace_back(modName);
	}

	if (m_useModOrganizer)
	{
		if (!findRealPaths(modsPathList)) // Convert the mods name to their real location inside ModOrganizer
			return false;
	}
	else
	{
		const auto& dataFolder = m_dataFolder.toStdString();
		for (auto& path : modsPathList)
			path = dataFolder + "/" + path;
	}

	return true;
}

bool ConfigModsParser::findRealPaths(std::vector<std::string>& paths)
{
	QString modsDirPath;
	if (m_modOrganizerPath.isEmpty() || m_pluginsListPath.isEmpty())
		return false;

	// Convert to a QStringList
	QStringList pathsList;
	for (const auto& path : paths)
		pathsList.push_back(convert(path));
	paths.clear();

	// Get the current profile
	QDir modOrganizerDir = QFileInfo(m_modOrganizerPath).absoluteDir();
	QFileInfo modOrganizerIni(modOrganizerDir, "ModOrganizer.ini"); 
	if (!modOrganizerIni.exists())
		return false;

	// Get the mod directory
	QSettings modOrganizerSettings(modOrganizerIni.absoluteFilePath(), QSettings::IniFormat);
	modsDirPath = modOrganizerSettings.value("Settings/mod_directory").toString();
	if (modsDirPath.isEmpty())
		return false;

	QDir modsDir(modsDirPath);
	if (!modsDir.exists())
		return false;

	// Get the mod list
	QDir modOrganizerProfileDir = QFileInfo(m_pluginsListPath).absoluteDir();
	QFileInfo modListFileInfo(modOrganizerProfileDir, "modlist.txt");
	if (!modListFileInfo.exists())
		return false;

	QFile modListFile(modListFileInfo.absoluteFilePath());
	if (!modListFile.open(QIODevice::ReadOnly))
		return false;

	using QStringPair = std::pair<QString, QString>;
	std::vector<QStringPair> realPathsPairs;
	QStringList filters;
	filters << "*.esm" << "*.esp";
	QTextStream modListStream(&modListFile);
	while (!modListStream.atEnd())
	{
		QString modName = modListStream.readLine();
		if (!modName.startsWith('+'))
			continue;
		modName = modName.mid(1); // Remove '+'

		QDir modDir(modsDir);
		if (!modDir.cd(modName))
			continue;

		QStringList files = modDir.entryList(filters, QDir::Files);
		for (const auto& path : pathsList)
		{
			if (files.contains(path))
				realPathsPairs.emplace_back(path, QFileInfo(modDir, path).absoluteFilePath());
		}
	}

	// Lastly, look into the Skyrim data folder
	QDir dataDir(m_dataFolder);
	QStringList files = dataDir.entryList(filters, QDir::Files);
	for (const auto& path : pathsList)
	{
		if (files.contains(path))
			realPathsPairs.emplace_back(path, QFileInfo(dataDir, path).absoluteFilePath());
	}

	for (const auto& path : pathsList)
	{
		auto it = std::find_if(realPathsPairs.begin(), realPathsPairs.end(), [&path](const QStringPair& p){
			return p.first == path;
		});
		if (it != realPathsPairs.end())
			paths.push_back(it->second.toStdString());
	}

	return true;
}

void ConfigModsParser::copyToConfig(Config& config)
{
	config.plugins.clear();
	config.effects.clear();
	config.ingredients.clear();

	// Add plugins
	for (const auto& inMod : m_config.modsList)
		config.plugins.emplace_back(convert(inMod));

	// Sort the plugins list by name
	std::sort(config.plugins.begin(), config.plugins.end(), [](const Config::Plugin& lhs, const Config::Plugin& rhs){
		return lhs.name < rhs.name;
	});

	// Add effects
	for (const auto& inEffect : m_config.magicalEffectsList)
	{
		Config::Effect effect;
		effect.code = inEffect.id;
		effect.flags = inEffect.flags;
		effect.baseCost = inEffect.baseCost;
		effect.name = convert(inEffect.name);
		effect.description = convert(inEffect.description);
		config.effects.push_back(effect);
	}

	// Sort the effects list by name
	std::sort(config.effects.begin(), config.effects.end(), [](const Config::Effect& lhs, const Config::Effect& rhs){
		return lhs.name < rhs.name;
	});

	// Add ingredients
	for (const auto& inIng : m_config.ingredientsList)
	{
		Config::Ingredient ingredient;
		ingredient.code = inIng.id;
		ingredient.name = convert(inIng.name);
		ingredient.pluginId = config.indexOfPlugin(convert(inIng.modName));
		if (ingredient.pluginId == -1)
			continue;
		++config.plugins[ingredient.pluginId].nbIngredients;

		bool validEffects = true;
		for (int i = 0; i < 4; ++i)
		{
			Config::EffectData& effectData = ingredient.effects[i];
			effectData.effectId = config.indexOfEffect(inIng.effects[i].id);
			if (effectData.effectId == -1)
			{
				validEffects = false;
				break;
			}
			effectData.magnitude = inIng.effects[i].magnitude;
			effectData.duration = inIng.effects[i].duration;
		}
		if (!validEffects)
			continue;

		// Sort the effects for an easier computation of potions,
		//  but keep the original ones as they are for the known ingredients loaded for each save
		std::copy(std::begin(ingredient.effects), std::end(ingredient.effects), std::begin(ingredient.sortedEffects));
		std::sort(std::begin(ingredient.sortedEffects), std::end(ingredient.sortedEffects),
			[](const Config::EffectData& lhs, const Config::EffectData& rhs)
		{ return lhs.effectId < rhs.effectId; }
		);

		config.ingredients.push_back(ingredient);
	}

	// Sort the ingredients list by name
	std::sort(config.ingredients.begin(), config.ingredients.end(), [](const Config::Ingredient& lhs, const Config::Ingredient& rhs){
		return lhs.name < rhs.name;
	});

	// Create the tooltips of the effects (with the sorted list of ingredients)
	for (int i = 0, nb = config.ingredients.size(); i < nb; ++i)
	{
		auto& ingredient = config.ingredients[i];
		for (auto effectData : ingredient.sortedEffects)
		{
			auto& effect = config.effects[effectData.effectId];
			ingredient.tooltip += effect.name + "\n";
			effect.ingredients.push_back(i);
			effect.tooltip += ingredient.name + "\n";
		}
		ingredient.tooltip = ingredient.tooltip.trimmed();
	}

	// Trim the tooltips of the effects
	for (auto& effect : config.effects)
		effect.tooltip = effect.tooltip.trimmed();
}