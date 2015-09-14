#include <QtWidgets>

#include <fstream>
#include <sstream>

#include "ConfigPane.h"
#include "Config.h"
#include "ConfigModsParser.h"
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

}

ConfigPane::ConfigPane(Config& config, bool firstLaunch, QWidget *parent)
	: QWidget(parent)
	, m_firstLaunch(firstLaunch)
	, m_config(config)
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

	auto languageLabel = new QLabel(tr("Language"));
	m_languageEdit = new QLineEdit;
	gridLayout->addWidget(languageLabel, 5, 0);
	gridLayout->addWidget(m_languageEdit, 5, 1);

	auto buttonsLayout = new QHBoxLayout;
	auto parseModsButton = new QPushButton(tr("Parse mods"));
	connect(parseModsButton, SIGNAL(clicked()), this, SLOT(parseMods()));
	auto defaultConfigButton = new QPushButton(tr("Default configuration"));
	connect(defaultConfigButton, SIGNAL(clicked()), this, SLOT(defaultConfig()));
	buttonsLayout->addWidget(parseModsButton);
	buttonsLayout->addWidget(defaultConfigButton);
	buttonsLayout->addStretch();
	gridLayout->addLayout(buttonsLayout, 6, 0, 1, 3);

	auto stretchLayout = new QVBoxLayout;
	stretchLayout->addStretch();
	gridLayout->addLayout(stretchLayout, 7, 0);

	setLayout(gridLayout);

	
}

void ConfigPane::loadConfig()
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
	m_languageEdit->setText(settings.language);
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
					steamFolder = convert(folder);
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
	auto& settings = Settings::instance();
	settings.useModOrganizer = m_useModOrganizerCheckBox->checkState() == Qt::Checked;
	settings.dataFolder = m_dataFolderEdit->text();
	settings.pluginsListPath = m_pluginsListPathEdit->text();
	settings.savesFolder = m_savesFolderEdit->text();
	settings.modOrganizerPath = m_modOrganizerPathEdit->text();
	settings.language = m_languageEdit->text();
}

void ConfigPane::parseMods()
{
	ConfigModsParser modsParser(m_useModOrganizerCheckBox->checkState() == Qt::Checked,
		m_dataFolderEdit->text(), m_pluginsListPathEdit->text(), m_modOrganizerPathEdit->text(),
		m_languageEdit->text());

	auto result = modsParser.parse();

	switch (result)
	{
	case ConfigModsParser::Result::Error_ModOrganizer:
		QMessageBox::warning(this, tr("Mods list error"), tr("There was an error trying to find mods with ModOrganizer"));
		return;

	case ConfigModsParser::Result::Error_ModsParsing:
		QMessageBox::warning(this, tr("Error in parsing"), tr("There was an error trying to extract ingredients from the mods"));
		return;

	case ConfigModsParser::Result::Success:
		emit startModsParse();
		modsParser.copyToConfig(m_config);

		emit endModsParse();
		m_modified = true;

		QMessageBox::information(this, tr("Loading finished"), tr("%1 ingredients found in %2 mods")
			.arg(modsParser.nbIngredients()).arg(modsParser.nbPlugins()));
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
