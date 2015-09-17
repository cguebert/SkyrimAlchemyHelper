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
	if (!lenFrom) return;
	size_t pos = 0;
	while ((pos = text.find(from, pos)) != std::string::npos)
	{
		text.replace(pos, lenFrom, to);
		pos += lenTo;
	}
}

}

ConfigDialog::ConfigDialog(QWidget *parent, bool firstLaunch)
	: QDialog(parent)
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

	m_useModOrganizerCheckBox = new QCheckBox(tr("Use Mod Organizer"));
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
	m_languageComboBox = new QComboBox;
	QStringList languages;
	languages << "english" << "french" << "german" << "italian" << "spanish";
	m_languageComboBox->addItems(languages);
	gridLayout->addWidget(languageLabel, 5, 0);
	gridLayout->addWidget(m_languageComboBox, 5, 1);

	auto buttonsLayout = new QHBoxLayout;
	auto parseModsButton = new QPushButton(tr("Parse mods"));
	connect(parseModsButton, SIGNAL(clicked()), this, SLOT(parseMods()));
	auto defaultConfigButton = new QPushButton(tr("Auto configuration"));
	connect(defaultConfigButton, SIGNAL(clicked()), this, SLOT(defaultConfig()));
	buttonsLayout->addWidget(parseModsButton);
	buttonsLayout->addWidget(defaultConfigButton);
	buttonsLayout->addStretch();
	gridLayout->addLayout(buttonsLayout, 6, 0, 1, 3);

	auto stretchLayout = new QVBoxLayout;
	stretchLayout->addStretch();
	gridLayout->addLayout(stretchLayout, 7, 0);

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

	Config::main() = m_config;
	m_config.save();
}

void ConfigDialog::defaultConfig()
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
					steamFolder = QString::fromLatin1(folder.c_str());
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
		if (!info.exists())
			return false;
		if (!info.isExecutable())
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

		QMessageBox::information(this, tr("Loading finished"), tr("%1 ingredients found in %2 mods")
			.arg(modsParser.nbIngredients()).arg(modsParser.nbPlugins()));
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
	auto path = QFileDialog::getOpenFileName(this, tr("Mod Organizer path"), m_modOrganizerPathEdit->text(), tr("Mod Organizer (ModOrganizer.exe)"));
	if (!path.isNull())
		m_modOrganizerPathEdit->setText(path);
}

void ConfigDialog::useModOrganizerChanged(int state)
{
	m_modOrganizerPathEdit->setEnabled(state == Qt::Checked);
	m_modOrganizerPathButton->setEnabled(state == Qt::Checked);
}
