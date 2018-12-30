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

#include "ConfigDialog.h"
#include "EffectsListWidget.h"
#include "IngredientsListWidget.h"
#include "PluginsListWidget.h"

#include "ModsParserWrapper.h"
#include "Settings.h"

namespace
{
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
	if (!lenFrom)
		return;
	size_t pos = 0;
	while ((pos = text.find(from, pos)) != std::string::npos)
	{
		text.replace(pos, lenFrom, to);
		pos += lenTo;
	}
}

QString mostRecentGameUsed()
{
	QStringList names;
	names << "Skyrim"
		  << "Skyrim Special Edition"
		  << "Skyrim VR";

	QDateTime lastModified;
	QString gameName = "Skyrim";
	for (const auto name : names)
	{
		QString folder = QString("My Games/%1/Saves").arg(name);
		QString savesFolder = QStandardPaths::locate(QStandardPaths::DocumentsLocation, folder, QStandardPaths::LocateDirectory);
		if (savesFolder.isEmpty())
			continue;

		QDir dir(savesFolder);
		QFileInfoList entries = dir.entryInfoList(QDir::Filter::Files, QDir::SortFlag::Time);
		if (!entries.empty() && entries.front().lastModified() > lastModified)
		{
			gameName = name;
			lastModified = entries.front().lastModified();
		}
	}

	return gameName;
}

QString gameNameForModOrganizer(QString gameName)
{
	if (gameName == "Skyrim Special Edition")
		return "SkyrimSE";
	if (gameName == "Skyrim VR")
		return "SkyrimVR";
	return "Skyrim";
}

QString gameNameForSteam(QString gameName)
{
	if (gameName == "Skyrim VR")
		return "SkyrimVR";
	return gameName;
}

QString gameExecutable(QString gameName)
{
	if (gameName == "Skyrim Special Edition")
		return "SkyrimSE";
	if (gameName == "Skyrim VR")
		return "SkyrimVR";
	return "TESV";
}

} // namespace

ConfigDialog::ConfigDialog(QWidget* parent, bool firstLaunch)
	: QDialog(parent)
	, m_firstLaunch(firstLaunch)
{
	// Copy main lists
	m_config = Config::main();

	setWindowTitle("Skyrim Alchemy Helper - Config");
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(5, 5, 5, 5);

	auto tabWidget = new QTabWidget;
	vLayout->addWidget(tabWidget);

	auto configPane = createConfigPane();
	tabWidget->addTab(configPane, "General");

	auto pluginsWidget = new PluginsListWidget(m_config);
	tabWidget->addTab(pluginsWidget, "Plugins");

	auto effectsWidget = new EffectsListWidget(m_config);
	tabWidget->addTab(effectsWidget, "Effects");

	auto ingredientsWidget = new IngredientsListWidget(m_config);
	tabWidget->addTab(ingredientsWidget, "Ingredients");

	QPushButton* okButton = new QPushButton(tr("Ok"), this);
	connect(okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(okButton);
	buttonsLayout->addWidget(cancelButton);
	vLayout->addLayout(buttonsLayout);

	setLayout(vLayout);

	connect(this, SIGNAL(startModsParse()), pluginsWidget, SLOT(beginReset()));
	connect(this, SIGNAL(startModsParse()), effectsWidget, SLOT(beginReset()));
	connect(this, SIGNAL(startModsParse()), ingredientsWidget, SLOT(beginReset()));

	connect(this, SIGNAL(endModsParse()), pluginsWidget, SLOT(endReset()));
	connect(this, SIGNAL(endModsParse()), effectsWidget, SLOT(endReset()));
	connect(this, SIGNAL(endModsParse()), ingredientsWidget, SLOT(endReset()));

	loadConfig();
}

QWidget* ConfigDialog::createConfigPane()
{
	auto configPane = new QWidget;
	auto gridLayout = new QGridLayout;

	auto gameNameLabel = new QLabel(tr("Game name"));
	m_gameNameComboBox = new QComboBox;
	m_gameNameComboBox->addItems(QStringList()
								 << "Skyrim"
								 << "Skyrim Special Edition"
								 << "Skyrim VR");
	m_gameNameComboBox->setCurrentIndex(0);
	gridLayout->addWidget(gameNameLabel, 0, 0);
	gridLayout->addWidget(m_gameNameComboBox, 0, 1);

	auto dataLabel = new QLabel(tr("Skyrim Data folder"));
	m_dataFolderEdit = new QLineEdit;
	auto dataButton = new QPushButton("...");
	connect(dataButton, SIGNAL(clicked()), this, SLOT(editDataPath()));
	gridLayout->addWidget(dataLabel, 1, 0);
	gridLayout->addWidget(m_dataFolderEdit, 1, 1);
	gridLayout->addWidget(dataButton, 1, 2);

	auto pluginsListLabel = new QLabel(tr("Plugins list file"));
	m_pluginsListPathEdit = new QLineEdit;
	auto pluginListButton = new QPushButton("...");
	connect(pluginListButton, SIGNAL(clicked()), this, SLOT(editPluginsPath()));
	gridLayout->addWidget(pluginsListLabel, 2, 0);
	gridLayout->addWidget(m_pluginsListPathEdit, 2, 1);
	gridLayout->addWidget(pluginListButton, 2, 2);

	auto savesLabel = new QLabel(tr("Saves folder"));
	m_savesFolderEdit = new QLineEdit;
	auto savesButton = new QPushButton("...");
	connect(savesButton, SIGNAL(clicked()), this, SLOT(editSavesPath()));
	gridLayout->addWidget(savesLabel, 3, 0);
	gridLayout->addWidget(m_savesFolderEdit, 3, 1);
	gridLayout->addWidget(savesButton, 3, 2);

	m_useModOrganizerCheckBox = new QCheckBox(tr("Use Mod Organizer"));
	connect(m_useModOrganizerCheckBox, SIGNAL(stateChanged(int)), this, SLOT(useModOrganizerChanged(int)));
	gridLayout->addWidget(m_useModOrganizerCheckBox, 4, 0, 1, 3);

	auto modOrganizerLabel = new QLabel(tr("Mod Organizer configuration"));
	m_modOrganizerPathEdit = new QLineEdit;
	m_modOrganizerPathButton = new QPushButton("...");
	connect(m_modOrganizerPathButton, SIGNAL(clicked()), this, SLOT(editModOrganizerPath()));
	m_modOrganizerPathEdit->setEnabled(false);
	m_modOrganizerPathButton->setEnabled(false);
	gridLayout->addWidget(modOrganizerLabel, 5, 0);
	gridLayout->addWidget(m_modOrganizerPathEdit, 5, 1);
	gridLayout->addWidget(m_modOrganizerPathButton, 5, 2);

	auto languageLabel = new QLabel(tr("Language"));
	m_languageComboBox = new QComboBox;
	QStringList languages;
	languages << "english"
			  << "french"
			  << "german"
			  << "italian"
			  << "spanish";
	m_languageComboBox->addItems(languages);
	gridLayout->addWidget(languageLabel, 6, 0);
	gridLayout->addWidget(m_languageComboBox, 6, 1);

	auto buttonsLayout = new QHBoxLayout;
	auto parseModsButton = new QPushButton(tr("Parse mods"));
	connect(parseModsButton, SIGNAL(clicked()), this, SLOT(parseMods()));
	auto defaultConfigButton = new QPushButton(tr("Auto configuration"));
	connect(defaultConfigButton, SIGNAL(clicked()), this, SLOT(defaultConfig()));
	buttonsLayout->addWidget(parseModsButton);
	buttonsLayout->addWidget(defaultConfigButton);
	buttonsLayout->addStretch();
	gridLayout->addLayout(buttonsLayout, 7, 0, 1, 3);

	auto stretchLayout = new QVBoxLayout;
	stretchLayout->addStretch();
	gridLayout->addLayout(stretchLayout, 8, 0);

	configPane->setLayout(gridLayout);
	return configPane;
}

QSize ConfigDialog::sizeHint() const
{
	return QSize(850, 500);
}

bool ConfigDialog::modified() const
{
	return m_modified;
}

void ConfigDialog::onOk()
{
	if (testConfig())
	{
		saveConfig();
		accept();
	}
	else
	{
		auto button = QMessageBox::question(this, tr("Invalid configuration"),
											tr("The current configuration is not valid, save it anyway?"),
											QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if (button == QMessageBox::Yes)
		{
			saveConfig();
			accept();
		}
		else if (button == QMessageBox::No)
			reject();
	}
}

void ConfigDialog::loadConfig()
{
	auto& settings = Settings::instance();
	if (settings.isEmpty())
	{
		defaultConfig();
		return;
	}

	m_useModOrganizerCheckBox->setCheckState(settings.useModOrganizer ? Qt::Checked : Qt::Unchecked);
	m_modOrganizerPathEdit->setEnabled(settings.useModOrganizer);
	m_modOrganizerPathButton->setEnabled(settings.useModOrganizer);

	m_dataFolderEdit->setText(settings.dataFolder);
	m_pluginsListPathEdit->setText(settings.pluginsListPath);
	m_savesFolderEdit->setText(settings.savesFolder);
	m_modOrganizerPathEdit->setText(settings.modOrganizerPath);
	m_languageComboBox->setCurrentText(settings.language);
}

void ConfigDialog::saveConfig()
{
	auto& settings = Settings::instance();
	settings.useModOrganizer = m_useModOrganizerCheckBox->checkState() == Qt::Checked;
	settings.dataFolder = m_dataFolderEdit->text();
	settings.pluginsListPath = m_pluginsListPathEdit->text();
	settings.savesFolder = m_savesFolderEdit->text();
	settings.modOrganizerPath = m_modOrganizerPathEdit->text();
	settings.language = m_languageComboBox->currentText();

	settings.clearEmptyFlag();

	Config::main() = m_config;
	m_config.save();
}

void ConfigDialog::defaultConfig()
{
	const QString gameName = m_firstLaunch
								 ? mostRecentGameUsed() // Select the game name based on the most recent save
								 : m_gameNameComboBox->currentText();

	if (m_firstLaunch)
		m_gameNameComboBox->setCurrentText(gameName);

	// Skyrim Data folder
	m_dataFolderEdit->setText("");
	// Steam folder
	QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);
	QString steamFolder = settings.value("SteamPath").toString();
	if (!steamFolder.isEmpty())
	{
		const QString steamGameName = gameNameForSteam(gameName);
		const QString exeName = gameExecutable(gameName);
		QString exePath = QString("%1/steamapps/common/%2/%3.exe").arg(steamFolder).arg(steamGameName).arg(exeName);

		// Is Skyrim installed in the same folder as Steam?
		if (QFileInfo::exists(exePath))
			m_dataFolderEdit->setText(steamFolder + "/steamapps/common/" + steamGameName + "/Data");
		else
		{
			// Parse "config.vdf"
			auto content = loadFile(steamFolder.toStdString() + "/config/config.vdf");
			if (!content.empty())
			{
				// Get additionnal steam games folders
				size_t pos = 0;
				while (true)
				{
					pos = content.find("\"BaseInstallFolder_", pos);
					if (pos == std::string::npos)
						break;

					pos = content.find("\"", pos + 1);        // end of "BaseInstallFolder_xxx"
					auto start = content.find("\"", pos + 1); // start of folder name
					auto end = content.find("\"", start + 1); // end of folder name
					auto folder = content.substr(start + 1, end - start - 1);
					replaceAll(folder, "\\\\", "/");

					// Find Skyrim
					steamFolder = QString::fromLatin1(folder.c_str());
					exePath = QString("%1/steamapps/common/%2/%3.exe").arg(steamFolder).arg(steamGameName).arg(exeName);
					if (QFileInfo::exists(exePath))
					{
						m_dataFolderEdit->setText(steamFolder + "/steamapps/common/" + steamGameName + "/Data");
						break;
					}
				}
			}
		}
	}

	// Trying to find Mod Organizer
	if (m_modOrganizerPathEdit->text().isEmpty())
	{
		// Is there Mod Organizer in the user directory ?
		QString moIniPath = QStandardPaths::locate(QStandardPaths::GenericConfigLocation,
												   QString("ModOrganizer/%1/ModOrganizer.ini").arg(gameNameForModOrganizer(gameName)),
												   QStandardPaths::LocateFile);
		if (!moIniPath.isEmpty())
			m_modOrganizerPathEdit->setText(moIniPath);

		// If at the first launch of the program we find Mod Organizer, we try to use it
		if (QFileInfo::exists(m_modOrganizerPathEdit->text()))
		{
			if (m_firstLaunch)
				m_useModOrganizerCheckBox->setCheckState(Qt::Checked);
		}
		else
			m_useModOrganizerCheckBox->setCheckState(Qt::Unchecked);
	}

	m_pluginsListPathEdit->setText("");
	m_savesFolderEdit->setText("");

	bool useModOrganizer = (m_useModOrganizerCheckBox->checkState() == Qt::Checked);
	if (useModOrganizer)
	{
		QFileInfo modOrganizerIni(m_modOrganizerPathEdit->text());
		if (modOrganizerIni.exists())
		{
			QSettings modOrganizerSettings(modOrganizerIni.absoluteFilePath(), QSettings::IniFormat);
			QString profile = modOrganizerSettings.value("selected_profile").toString();
			QString profilesDirStr = modOrganizerSettings.value("Settings/profiles_directory", "%BASE_DIR%/profiles").toString();
			profilesDirStr.replace("%BASE_DIR%", modOrganizerIni.canonicalPath());

			if (!profile.isEmpty())
			{
				QFileInfo profileDir(profilesDirStr + "/" + profile);
				if (profileDir.exists() && profileDir.isDir())
					m_pluginsListPathEdit->setText(profileDir.canonicalFilePath() + "/plugins.txt");
			}
		}
		else
		{
			m_useModOrganizerCheckBox->setCheckState(Qt::Unchecked);
			useModOrganizer = false;
		}
	}

	if (!useModOrganizer) // Can be changed if the ModOrganizer configuration cannot be read
	{
		QString pluginsListPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, gameName + "/plugins.txt", QStandardPaths::LocateFile);
		if (!pluginsListPath.isEmpty())
			m_pluginsListPathEdit->setText(pluginsListPath);
	}

	QString savesFolder = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "My Games/" + gameName + "/Saves", QStandardPaths::LocateDirectory);
	if (!savesFolder.isEmpty())
		m_savesFolderEdit->setText(savesFolder);

	m_firstLaunch = false;
}

bool ConfigDialog::testConfig()
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
		if (info.fileName() != "ModOrganizer.ini")
			return false;

		QSettings modOrganizerSettings(info.absoluteFilePath(), QSettings::IniFormat);
		QString game = modOrganizerSettings.value("gameName").toString();
		if (!game.startsWith("Skyrim"))
			return false;
	}

	return true;
}

void ConfigDialog::parseMods()
{
	ModsParserWrapper modsParser(m_useModOrganizerCheckBox->checkState() == Qt::Checked,
								 m_dataFolderEdit->text(), m_pluginsListPathEdit->text(), m_modOrganizerPathEdit->text(),
								 m_languageComboBox->currentText());

	auto result = modsParser.parseConfig();

	switch (result)
	{
	case ModsParserWrapper::Result::Error_ModOrganizer:
		QMessageBox::warning(this, tr("Mods list error"), tr("There was an error trying to find mods with ModOrganizer"));
		return;

	case ModsParserWrapper::Result::Error_ModsParsing:
		QMessageBox::warning(this, tr("Error in parsing"), tr("There was an error trying to extract ingredients from the mods"));
		return;

	case ModsParserWrapper::Result::Success:
		emit startModsParse();
		modsParser.copyToConfig(m_config);
		emit endModsParse();
		m_modified = true;

		std::lock_guard<std::mutex> lock(ContainersCache::instance().containersMutex);
		ContainersCache::instance().containers.clear();

		QMessageBox::information(this, tr("Loading finished"), tr("%1 ingredients found in %2 mods").arg(modsParser.nbIngredients()).arg(modsParser.nbPlugins()));
	}
}

void ConfigDialog::editDataPath()
{
	auto path = QFileDialog::getExistingDirectory(this, tr("Skyrim Data Folder"), m_dataFolderEdit->text());
	if (!path.isNull())
		m_dataFolderEdit->setText(path);
}

void ConfigDialog::editPluginsPath()
{
	auto path = QFileDialog::getOpenFileName(this, tr("Plugins list path"), m_pluginsListPathEdit->text(), tr("Plugins File (plugins.txt)"));
	if (!path.isNull())
		m_pluginsListPathEdit->setText(path);
}

void ConfigDialog::editSavesPath()
{
	auto path = QFileDialog::getExistingDirectory(this, tr("Saves Folder"), m_savesFolderEdit->text());
	if (!path.isNull())
		m_savesFolderEdit->setText(path);
}

void ConfigDialog::editModOrganizerPath()
{
	auto path = QFileDialog::getOpenFileName(this, tr("Mod Organizer configuration path"), m_modOrganizerPathEdit->text(), tr("Mod Organizer configuration (ModOrganizer.ini)"));
	if (!path.isNull())
		m_modOrganizerPathEdit->setText(path);
}

void ConfigDialog::useModOrganizerChanged(int state)
{
	m_modOrganizerPathEdit->setEnabled(state == Qt::Checked);
	m_modOrganizerPathButton->setEnabled(state == Qt::Checked);
}
