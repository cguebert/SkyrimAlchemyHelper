#include <QtWidgets>

#include <fstream>
#include <sstream>

#include "ConfigPane.h"
#include "Config.h"
#include "EffectsList.h"
#include "IngredientsList.h"
#include "PluginsList.h"

#include <modParser/ModParser.h>

ConfigPane::ConfigPane(IngredientsList& ingredientsList,
	EffectsList& effectsList,
	PluginsList& pluginsList,
	bool firstLaunch,
	QWidget *parent)
	: QWidget(parent)
	, m_firstLaunch(firstLaunch)
	, m_ingredientsList(ingredientsList)
	, m_effectsList(effectsList)
	, m_pluginsList(pluginsList)
{
	auto gridLayout = new QGridLayout;

	auto dataLabel = new QLabel(tr("Skyrim Data folder"));
	m_dataFolderEdit = new QLineEdit;
	auto dataButton = new QPushButton("...");
	connect(dataButton, SIGNAL(clicked()), this, SLOT(editDataPath()));
	gridLayout->addWidget(dataLabel, 0, 0);
	gridLayout->addWidget(m_dataFolderEdit, 0, 1);
	gridLayout->addWidget(dataButton, 0, 2);

	auto pluginsListLabel = new QLabel(tr("Plugins list file"));
	m_pluginsListPathEdit = new QLineEdit;
	auto pluginListButton = new QPushButton("...");
	connect(pluginListButton, SIGNAL(clicked()), this, SLOT(editPluginsPath()));
	gridLayout->addWidget(pluginsListLabel, 1, 0);
	gridLayout->addWidget(m_pluginsListPathEdit, 1, 1);
	gridLayout->addWidget(pluginListButton, 1, 2);

	auto savesLabel = new QLabel(tr("Saves folder"));
	m_savesFolderEdit = new QLineEdit;
	auto savesButton = new QPushButton("...");
	connect(savesButton, SIGNAL(clicked()), this, SLOT(editSavesPath()));
	gridLayout->addWidget(savesLabel, 2, 0);
	gridLayout->addWidget(m_savesFolderEdit, 2, 1);
	gridLayout->addWidget(savesButton, 2, 2);

	m_useModOrganizerCheckBox = new QCheckBox("Use Mod Organizer");
	connect(m_useModOrganizerCheckBox, SIGNAL(stateChanged(int)), this, SLOT(useModOrganizerChanged(int)));
	gridLayout->addWidget(m_useModOrganizerCheckBox, 3, 0, 1, 3);

	auto modOrganizerLabel = new QLabel(tr("Mod Organizer"));
	m_modOrganizerPathEdit = new QLineEdit;
	m_modOrganizerPathButton = new QPushButton("...");
	connect(m_modOrganizerPathButton, SIGNAL(clicked()), this, SLOT(editModOrganizerPath()));
	m_modOrganizerPathEdit->setEnabled(false);
	m_modOrganizerPathButton->setEnabled(false);
	gridLayout->addWidget(modOrganizerLabel, 4, 0);
	gridLayout->addWidget(m_modOrganizerPathEdit, 4, 1);
	gridLayout->addWidget(m_modOrganizerPathButton, 4, 2);

	auto buttonsLayout = new QHBoxLayout;
	auto parseModsButton = new QPushButton(tr("Parse mods"));
	connect(parseModsButton, SIGNAL(clicked()), this, SLOT(parseMods()));
	auto defaultConfigButton = new QPushButton(tr("Default configuration"));
	connect(defaultConfigButton, SIGNAL(clicked()), this, SLOT(defaultConfig()));
	buttonsLayout->addWidget(parseModsButton);
	buttonsLayout->addWidget(defaultConfigButton);
	buttonsLayout->addStretch();
	gridLayout->addLayout(buttonsLayout, 5, 0, 1, 3);

	auto stretchLayout = new QVBoxLayout;
	stretchLayout->addStretch();
	gridLayout->addLayout(stretchLayout, 6, 0);

	setLayout(gridLayout);

	loadConfig();
}

void ConfigPane::loadConfig()
{
	auto& config = Config::instance();
	if (config.isEmpty())
	{
		defaultConfig();
		return;
	}

	m_useModOrganizerCheckBox->setCheckState(config.useModOrganizer ? Qt::Checked : Qt::Unchecked);
	m_modOrganizerPathEdit->setEnabled(config.useModOrganizer);
	m_modOrganizerPathButton->setEnabled(config.useModOrganizer);

	m_dataFolderEdit->setText(config.dataFolder);
	m_pluginsListPathEdit->setText(config.pluginsListPath);
	m_savesFolderEdit->setText(config.savesFolder);
	m_modOrganizerPathEdit->setText(config.modOrganizerPath);
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

void replaceAll(std::string& text, const std::string& from, const std::string& to)
{
	size_t lenFrom = from.size(), lenTo = to.size();
	if (!lenFrom) return;
	size_t pos = 0;
	while ((pos = text.find(from, pos)) != std::string::npos)
	{
		text.replace(pos, lenFrom, to);
		pos += lenTo;
	}
}

void ConfigPane::defaultConfig()
{
	// Skyrim Data folder
	m_dataFolderEdit->setText("");
	//   Steam folder
	QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);
	QString steamFolder = settings.value("SteamPath").toString();
	if (!steamFolder.isEmpty())
	{
		//   Is Skyrim installed in the same folder as Steam?
		if (QFileInfo::exists(steamFolder + "/steamapps/common/Skyrim/TESV.exe"))
			m_dataFolderEdit->setText(steamFolder + "/steamapps/common/Skyrim/Data");
		else
		{
			//   Parse "config.vdf"
			auto content = loadFile(steamFolder.toStdString() + "/config/config.vdf");
			if (!content.empty())
			{
				//   Get additionnal steam games folders
				size_t pos = 0;
				while (true)
				{
					pos = content.find("\"BaseInstallFolder_", pos);
					if (pos == std::string::npos)
						break;

					pos = content.find("\"", pos + 1); // end of "BaseInstallFolder_xxx"
					auto start = content.find("\"", pos + 1); // start of folder name
					auto end = content.find("\"", start + 1); // end of folder name
					auto folder = content.substr(start + 1, end - start - 1);
					replaceAll(folder, "\\\\", "/");

					//   Find Skyrim
					steamFolder = folder.c_str();
					if (QFileInfo::exists(steamFolder + "/steamapps/common/Skyrim/TESV.exe"))
					{
						m_dataFolderEdit->setText(steamFolder + "/steamapps/common/Skyrim/Data");
						break;
					}
				}
			}
		}
	}

	// Trying to find Mod Organizer
	if (m_modOrganizerPathEdit->text().isEmpty())
	{
		// First try at the standard location
		auto standardPath = "C:/Program Files (x86)/Mod Organizer/ModOrganizer.exe";
		if (QFileInfo::exists(standardPath))
			m_modOrganizerPathEdit->setText(standardPath);
		else // Maybe Mod Organizer has been set as the handler of nxm web links
		{
			QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Classes", QSettings::NativeFormat);
			QString command = settings.value("nxm/shell/open/command/.").toString();
			if (command.contains("nxmhandler.exe"))
			{
				auto pos = command.lastIndexOf("\\");
				if (pos != -1)
				{
					QString dir = command.left(pos);
					dir.replace("\\", "/");
					QString path = dir + "/ModOrganizer.exe";
					if (QFileInfo::exists(path))
						m_modOrganizerPathEdit->setText(path);
				}
			}
		}

		// If at the first launch of the program we find Mod Organizer, we try to use it
		if (m_firstLaunch && !m_modOrganizerPathEdit->text().isEmpty())
			m_useModOrganizerCheckBox->setCheckState(Qt::Checked);
	}

	m_pluginsListPathEdit->setText("");
	m_savesFolderEdit->setText("");

	bool useModOrganizer = (m_useModOrganizerCheckBox->checkState() == Qt::Checked);
	if (useModOrganizer)
	{
		QString modOrganizerPath = m_modOrganizerPathEdit->text();
		if (!modOrganizerPath.isEmpty())
		{
			QDir modOrganizerDir = QFileInfo(modOrganizerPath).absoluteDir();
			QFileInfo modOrganizerIni(modOrganizerDir, "ModOrganizer.ini"); // We read the settings to get the current profile
			if (modOrganizerIni.exists())
			{
				QSettings modOrganizerSettings(modOrganizerIni.absoluteFilePath(), QSettings::IniFormat);
				QString profile = modOrganizerSettings.value("selected_profile").toString();
				if (!profile.isEmpty())
				{
					modOrganizerDir.cd("profiles");
					modOrganizerDir.cd(profile);
					if (modOrganizerDir.exists())
					{
						m_pluginsListPathEdit->setText(modOrganizerDir.absolutePath() + "/plugins.txt");
						m_savesFolderEdit->setText(modOrganizerDir.absolutePath() + "/saves");
					}
				}
			}
			else
			{
				m_useModOrganizerCheckBox->setCheckState(Qt::Unchecked);
				useModOrganizer = false;
			}
		}
	}
	
	if (!useModOrganizer) // Can be changed if the ModOrganizer configuration cannot be read
	{
		QString pluginsListPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "Skyrim/plugins.txt", QStandardPaths::LocateFile);
		if (!pluginsListPath.isEmpty())
			m_pluginsListPathEdit->setText(pluginsListPath);

		QString savesFolder = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "My Games/Skyrim/Saves", QStandardPaths::LocateDirectory);
		if (!savesFolder.isEmpty())
			m_savesFolderEdit->setText(savesFolder);
	}

	m_firstLaunch = false;
}

bool ConfigPane::testConfig()
{
	if (!QFileInfo::exists(m_dataFolderEdit->text() + "/Skyrim.esm"))
		return false;

	if (!QFileInfo::exists(m_pluginsListPathEdit->text()))
		return false;

	QStringList filters;
	filters.push_back("*.ess");
	QDir saveDir(m_savesFolderEdit->text());
	if (saveDir.entryList(filters).empty())
		return false;

	if (m_useModOrganizerCheckBox->checkState() == Qt::Checked)
	{
		QFileInfo info(m_modOrganizerPathEdit->text());
		if (!info.exists())
			return false;
		if (!info.isExecutable())
			return false;
	}

	return true;
}

void ConfigPane::saveConfig()
{
	auto& config = Config::instance();
	config.useModOrganizer = m_useModOrganizerCheckBox->checkState() == Qt::Checked;
	config.dataFolder = m_dataFolderEdit->text();
	config.pluginsListPath = m_pluginsListPathEdit->text();
	config.savesFolder = m_savesFolderEdit->text();
	config.modOrganizerPath = m_modOrganizerPathEdit->text();
}

bool ConfigPane::getModsPaths(std::vector<std::string>& modsPathList)
{
	std::string dataDirPath = m_dataFolderEdit->text().toStdString();
	std::string pluginsListPath = m_pluginsListPathEdit->text().toStdString();
	
	// Add "Skyrim.esm" if not using Mod Organizer
	if (m_useModOrganizerCheckBox->checkState() != Qt::Checked)
	{
		std::string inList = loadFile(pluginsListPath);
		std::transform(inList.begin(), inList.end(), inList.begin(), ::tolower);
		if (inList.find("skyrim.esm") == std::string::npos)
			modsPathList.emplace_back("Skyrim.esm");
	}

	// Loading the "plugins.txt" file
	std::ifstream inFile(pluginsListPath);
	std::string modName;
	while (std::getline(inFile, modName))
	{
		if (modName[0] == '#')
			continue;

		modsPathList.emplace_back(modName);
	}

	if (m_useModOrganizerCheckBox->checkState() == Qt::Checked)
	{
		if (!findRealPaths(modsPathList)) // Convert the mods name to their real location inside ModOrganizer
		{
			QMessageBox::warning(this, tr("Mods list error"), tr("There was an error trying to find mods with ModOrganizer"));
			return false;
		}
	}
	else
	{
		for (auto& path : modsPathList)
			path = dataDirPath + "/" + path;
	}

	return true;
}

void ConfigPane::parseMods()
{
	std::vector<std::string> modsPathList;
	if (!getModsPaths(modsPathList))
		return;

	modParser::ModParser modParser;
	modParser.setModsList(modsPathList);
	auto config = modParser.parseConfig();

	if (config.ingredientsList.empty())
	{
		QMessageBox::warning(this, tr("Error in parsing"), tr("There was an error trying to extract ingredients from the mods"));
		return;
	}
	else
	{
		emit startModsParse();
		convertConfig(config);
		emit endModsParse();
		m_modified = true;

		QMessageBox::information(this, tr("Loading finished"), tr("%1 ingredients found in %2 mods").arg(m_ingredientsList.size()).arg(m_pluginsList.size()));
	}
}

void ConfigPane::editDataPath()
{
	auto path = QFileDialog::getExistingDirectory(this, tr("Skyrim Data Folder"), m_dataFolderEdit->text());
	if (!path.isNull())
		m_dataFolderEdit->setText(path);
}

void ConfigPane::editPluginsPath()
{
	auto path = QFileDialog::getOpenFileName(this, tr("Plugins list path"), m_pluginsListPathEdit->text(), tr("Plugins File (plugins.txt)"));
	if (!path.isNull())
		m_pluginsListPathEdit->setText(path);
}

void ConfigPane::editSavesPath()
{
	auto path = QFileDialog::getExistingDirectory(this, tr("Saves Folder"), m_savesFolderEdit->text());
	if (!path.isNull())
		m_savesFolderEdit->setText(path);
}

void ConfigPane::editModOrganizerPath()
{
	auto path = QFileDialog::getOpenFileName(this, tr("Mod Organizer path"), m_modOrganizerPathEdit->text(), tr("Mod Organizer (ModOrganizer.exe)"));
	if (!path.isNull())
		m_modOrganizerPathEdit->setText(path);
}

void ConfigPane::useModOrganizerChanged(int state)
{
	m_modOrganizerPathEdit->setEnabled(state == Qt::Checked);
	m_modOrganizerPathButton->setEnabled(state == Qt::Checked);
}

bool ConfigPane::modified()
{
	return m_modified;
}

bool ConfigPane::findRealPaths(std::vector<std::string>& paths)
{
	QString modsDirPath;
	QString modOrganizerPath = m_modOrganizerPathEdit->text();
	QString pluginsListPath = m_pluginsListPathEdit->text();
	if (modOrganizerPath.isEmpty() || pluginsListPath.isEmpty())
		return false;

	// Convert to a QStringList
	QStringList pathsList;
	for (const auto& path : paths)
		pathsList.push_back(path.c_str());
	paths.clear();

	// Get the current profile
	QDir modOrganizerDir = QFileInfo(modOrganizerPath).absoluteDir();
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
	QDir modOrganizerProfileDir = QFileInfo(pluginsListPath).absoluteDir();
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
	QDir dataDir(m_dataFolderEdit->text());
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

int indexOf(const EffectsList::Effects& effects, quint32 id)
{
	auto it = std::find_if(effects.begin(), effects.end(), [&id](const EffectsList::Effect& effect){
		return effect.code == id;
	});
	if (it != effects.end())
		return it - effects.begin();
	return -1;
}

int indexOf(const PluginsList::Plugins& plugins, QString name)
{
	auto it = std::find_if(plugins.begin(), plugins.end(), [&name](const PluginsList::Plugin& plugin){
		return !plugin.name.compare(name, Qt::CaseInsensitive);
	});
	if (it != plugins.end())
		return it - plugins.begin();
	return -1;
}

void ConfigPane::convertConfig(const modParser::Config& config)
{
	auto& plugins = m_pluginsList.plugins();
	auto& effects = m_effectsList.effects();
	auto& ingredients = m_ingredientsList.ingredients();

	plugins.clear();
	effects.clear();
	ingredients.clear();

	// Add plugins
	for (const auto& inMod : config.modsList)
		plugins.emplace_back(inMod.c_str());

	// Sort the plugins list by name
	std::sort(plugins.begin(), plugins.end(), [](const PluginsList::Plugin& lhs, const PluginsList::Plugin& rhs){
		return lhs.name < rhs.name;
	});

	// Add effects
	for (const auto& inEffect : config.magicalEffectsList)
	{
		EffectsList::Effect effect;
		effect.code = inEffect.id;
		effect.flags = inEffect.flags;
		effect.baseCost = inEffect.baseCost;
		effect.name = inEffect.name.c_str();
		effect.description = inEffect.description.c_str();
		effects.push_back(effect);
	}

	// Sort the effects list by name
	std::sort(effects.begin(), effects.end(), [](const EffectsList::Effect& lhs, const EffectsList::Effect& rhs){
		return lhs.name < rhs.name;
	});

	// Add ingredients
	for (const auto& inIng : config.ingredientsList)
	{
		IngredientsList::Ingredient ingredient;
		ingredient.code = inIng.id;
		ingredient.name = inIng.name.c_str();
		ingredient.pluginId = indexOf(plugins, inIng.modName.c_str());
		if (ingredient.pluginId == -1)
			continue;
		++plugins[ingredient.pluginId].nbIngredients;

		bool validEffects = true;
		for (int i = 0; i < 4; ++i)
		{
			IngredientsList::EffectData& effectData = ingredient.effects[i];
			effectData.effectId = indexOf(effects, inIng.effects[i].id);
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
			[](const IngredientsList::EffectData& lhs, const IngredientsList::EffectData& rhs)
			{ return lhs.effectId < rhs.effectId; }
		);

		ingredients.push_back(ingredient);
	}

	// Sort the ingredients list by name
	std::sort(ingredients.begin(), ingredients.end(), [](const IngredientsList::Ingredient& lhs, const IngredientsList::Ingredient& rhs){
		return lhs.name < rhs.name;
	});

	// Create the tooltips of the effects (with the sorted list of ingredients)
	for (int i = 0, nb = ingredients.size(); i < nb; ++i)
	{
		auto& ingredient = ingredients[i];
		for (auto effectData : ingredient.sortedEffects)
		{
			auto& effect = effects[effectData.effectId];
			ingredient.tooltip += effect.name + "\n";
			effect.ingredients.push_back(i);
			effect.tooltip += ingredient.name + "\n";
		}
		ingredient.tooltip = ingredient.tooltip.trimmed();
	}

	// Trim the tooltips of the effects
	for (auto& effect : effects)
		effect.tooltip = effect.tooltip.trimmed();
}
