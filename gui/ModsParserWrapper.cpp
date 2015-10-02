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
#include <QtWidgets>

#include <fstream>
#include <sstream>
#include <iostream>

#include "ModsParserWrapper.h"
#include "Settings.h"

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

std::ostream& logFile()
{
	static std::ofstream file;
	if (!file.is_open())
	{
		file.open("data/log.txt", std::ios_base::trunc);
		std::cout.rdbuf(file.rdbuf());
		std::cerr.rdbuf(file.rdbuf());
	}
	return file;
}

}

ModsParserWrapper::ModsParserWrapper()
{
	const auto& settings = Settings::instance();
	m_useModOrganizer = settings.useModOrganizer;
	m_dataFolder = settings.dataFolder;
	m_pluginsListPath = settings.pluginsListPath;
	m_modOrganizerPath = settings.modOrganizerPath;
	m_language = settings.language;
}

ModsParserWrapper::ModsParserWrapper(bool useModOrganizer,
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

ModsParserWrapper::Result ModsParserWrapper::parseConfig()
{
	logFile() << "*** Parse Config ***" << std::endl;

	std::vector<std::string> modsPathList;
	if (!getModsPaths(modsPathList))
	{
		logFile() << "Could not get all necessary information from ModOrganizer configuration" << std::endl << std::endl;
		return Result::Error_ModOrganizer;
	}

	modParser::ModParser modParser;
	modParser.setModsList(modsPathList);
	modParser.setLanguage(m_language.toStdString());
	m_config = modParser.parseConfig();

	if (m_config.ingredientsList.empty())
	{
		logFile() << "Error: parsing found no ingredients" << std::endl << std::endl;
		return Result::Error_ModsParsing;
	}

	logFile() << "Parsing successful" << std::endl << std::endl;

	return Result::Success;
}

bool ModsParserWrapper::getModsPaths(std::vector<std::string>& modsPathList)
{
	// Add "Skyrim.esm" if not using Mod Organizer
	if (!m_useModOrganizer)
	{
		std::string inList = loadFile(m_pluginsListPath.toStdString());
		std::transform(inList.begin(), inList.end(), inList.begin(), ::tolower);
		if (inList.find("skyrim.esm") == std::string::npos)
		{
			modsPathList.emplace_back("Skyrim.esm");
			logFile() << "Adding Skyrim.esm wich was not in the mods list" << std::endl;
		}
	}

	// Loading the "plugins.txt" file
	std::ifstream inFile(m_pluginsListPath.toStdString());
	std::string modName;
	while (std::getline(inFile, modName))
	{
		if (modName[0] == '#')
			continue;

		modsPathList.emplace_back(modName);
	}

	logFile() << modsPathList.size() << " mods found in " << m_pluginsListPath.toStdString() << std::endl;

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

bool ModsParserWrapper::findRealPaths(std::vector<std::string>& paths)
{
	QString modsDirPath;
	if (m_modOrganizerPath.isEmpty())
	{
		logFile() << "Error: path to ModOrganizer is empty" << std::endl;
		return false;
	}

	if (m_pluginsListPath.isEmpty())
	{
		logFile() << "Error: path to the plugins list file is empty" << std::endl;
		return false;
	}

	// Convert to a QStringList
	QStringList pathsList;
	for (const auto& path : paths)
		pathsList.push_back(convert(path));
	paths.clear();

	// Get the current profile
	QDir modOrganizerDir = QFileInfo(m_modOrganizerPath).absoluteDir();
	QFileInfo modOrganizerIni(modOrganizerDir, "ModOrganizer.ini"); 
	if (!modOrganizerIni.exists())
	{
		logFile() << "Error: cannot find " << modOrganizerIni.absoluteFilePath().toStdString() << std::endl;
		return false;
	}

	// Get the mod directory
	QSettings modOrganizerSettings(modOrganizerIni.absoluteFilePath(), QSettings::IniFormat);
	modsDirPath = modOrganizerSettings.value("Settings/mod_directory").toString();
	if (modsDirPath.isEmpty())
	{
		logFile() << "Warning: mod_directory setting not found, looking into \"mods\" directory" << std::endl;
		QDir modsDir(modOrganizerDir);
		if (!modsDir.cd("mods"))
		{
			logFile() << "Error: cannot find where the mods are installed with ModOrganizer" << std::endl;
			return false;
		}
		modsDirPath = modsDir.absolutePath();
	}

	QDir modsDir(modsDirPath);
	if (!modsDir.exists())
	{
		logFile() << "Error: failed to find mods in " << modsDirPath.toStdString() << std::endl;
		return false;
	}

	// Get the mod list
	QDir modOrganizerProfileDir = QFileInfo(m_pluginsListPath).absoluteDir();
	QFileInfo modListFileInfo(modOrganizerProfileDir, "modlist.txt");
	if (!modListFileInfo.exists())
	{
		logFile() << "Error: cannot find modlist.txt in " << modOrganizerProfileDir.absolutePath().toStdString() << std::endl;
		return false;
	}

	QFile modListFile(modListFileInfo.absoluteFilePath());
	if (!modListFile.open(QIODevice::ReadOnly))
	{
		logFile() << "Error: cannot open " << modListFileInfo.absoluteFilePath().toStdString() << std::endl;
		return false;
	}

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

	logFile() << "Successfully found the mods in the ModOrganizer configuration" << std::endl;
	return true;
}

void ModsParserWrapper::copyToConfig(Config& config) const
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

ModsParserWrapper::Result ModsParserWrapper::updateContainers(const std::vector<uint32_t>& ids)
{
	std::vector<std::string> modsPathList;
	if (!getModsPaths(modsPathList))
		return Result::Error_ModOrganizer;

	modParser::ModParser modParser;
	modParser.setModsList(modsPathList);
	modParser.setLanguage(m_language.toStdString());
	
	auto containers = modParser.getContainersInfo(ids);
	m_containers.clear();
	m_containers.reserve(containers.size());

	for (const auto& c : containers)
	{
		if (c.type.empty() || c.location.empty())
			continue;

		ContainersCache::Container container;
		container.code = c.id;
		container.cellCode = c.cell;
		container.interior = c.interior;
		container.name = convert(c.type);
		container.location = convert(c.location);
		m_containers.push_back(container);
	}

	if (!ids.empty() && containers.empty())
		return Result::Error_ModsParsing;

	return Result::Success;
}
