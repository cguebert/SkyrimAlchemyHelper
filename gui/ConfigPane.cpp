#include <QtWidgets>

#include <fstream>
#include <sstream>

#include "ConfigPane.h"
#include "EffectsList.h"
#include "IngredientsList.h"
#include "PluginsList.h"

ConfigPane::ConfigPane(QWidget *parent, bool firstLaunch)
	: QWidget(parent)
	, m_firstLaunch(firstLaunch)
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
	auto reloadListsButton = new QPushButton(tr("Reload lists"));
	connect(reloadListsButton, SIGNAL(clicked()), this, SLOT(updateLists()));
	buttonsLayout->addWidget(parseModsButton);
	buttonsLayout->addWidget(defaultConfigButton);
	buttonsLayout->addWidget(reloadListsButton);
	buttonsLayout->addStretch();
	gridLayout->addLayout(buttonsLayout, 5, 0, 1, 3);

	auto stretchLayout = new QVBoxLayout;
	stretchLayout->addStretch();
	gridLayout->addLayout(stretchLayout, 6, 0);

	setLayout(gridLayout);

	loadConfig();

	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(updateLists()));
}

void ConfigPane::loadConfig()
{
	QSettings settings;
	if (!settings.contains("useModOrganizer"))
	{
		defaultConfig();
		return;
	}

	bool useModOrganizer = settings.value("useModOrganizer").toBool();
	m_useModOrganizerCheckBox->setCheckState(useModOrganizer ? Qt::Checked : Qt::Unchecked);
	m_modOrganizerPathEdit->setEnabled(useModOrganizer);
	m_modOrganizerPathButton->setEnabled(useModOrganizer);

	m_dataFolderEdit->setText(settings.value("skyrimDataFolder").toString());
	m_pluginsListPathEdit->setText(settings.value("pluginsListFile").toString());
	m_savesFolderEdit->setText(settings.value("savesFolder").toString());
	m_modOrganizerPathEdit->setText(settings.value("modOrganizerPath").toString());
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

	if (m_useModOrganizerCheckBox->checkState() == Qt::Checked)
	{
		QString modOrganizerPath = m_modOrganizerPathEdit->text();
		if (!modOrganizerPath.isEmpty())
		{
			QDir modOrganizerDir = QFileInfo(modOrganizerPath).absoluteDir();
			if (modOrganizerDir.exists())
			{
				modOrganizerDir.cd("profiles");
				auto profiles = modOrganizerDir.entryInfoList(QDir::Dirs, QDir::Time);
				for (auto profile : profiles)
				{
					QString tmp = profile.absoluteFilePath();
					if (profile.baseName().isEmpty()) // Remove "." & ".."
						continue;

					m_pluginsListPathEdit->setText(profile.absoluteFilePath() + "/plugins.txt");
					m_savesFolderEdit->setText(profile.absoluteFilePath() + "/saves");
					break;
				}
			}
		}
	}
	else
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
	QSettings settings;
	settings.setValue("useModOrganizer", m_useModOrganizerCheckBox->checkState() == Qt::Checked);

	settings.setValue("skyrimDataFolder", m_dataFolderEdit->text());
	settings.setValue("pluginsListFile", m_pluginsListPathEdit->text());
	settings.setValue("savesFolder", m_savesFolderEdit->text());
	settings.setValue("modOrganizerPath", m_modOrganizerPathEdit->text());
}

bool ConfigPane::prepareParsing()
{
	QString appDir = QCoreApplication::applicationDirPath();

	// Prepare the "data/paths.txt" file
	QDir dataDir(appDir);
	if (!dataDir.cd("data"))
	{
		if (!dataDir.mkdir("data"))
		{
			QMessageBox::warning(this, tr("Access error"), tr("Cannot create the data folder"));
			return false;
		}

		dataDir.cd("data");
	}

	QFile file(dataDir.absolutePath() + "/paths.txt");
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(this, tr("Access error"), tr("Cannot write to data/paths.txt"));
		return false;
	}

	QDir::setCurrent(appDir);

	QTextStream stream(&file);
	stream << m_pluginsListPathEdit->text() << "\n"
		<< m_dataFolderEdit->text() << "\n";

	return true;
}

void ConfigPane::parseMods()
{
	if (!prepareParsing())
		return;

	QString modParserPath = QCoreApplication::applicationDirPath() +"/ModParser.exe";
	int result = 0;

	QString modOrganizerPath = m_modOrganizerPathEdit->text();
	if (m_useModOrganizerCheckBox->checkState() == Qt::Checked && !modOrganizerPath.isEmpty() && QFileInfo::exists(modOrganizerPath))
		result = QProcess::execute(modOrganizerPath + " " + modParserPath);
	else
		result = QProcess::execute(modParserPath);

	if (result)
	{
		QMessageBox::warning(this, tr("Error in parsing"), tr("There was an error trying to extract ingredients from the plugins"));
		return;
	}

	m_timer->start(3000); // Files can still be locked for now, I have found that waiting 3 seconds is enough for me
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

void ConfigPane::updateLists()
{
	emit startModsParse();

	PluginsList::GetInstance().loadList();
	EffectsList::GetInstance().loadList();
	IngredientsList::GetInstance().loadList();

	emit endModsParse();
}