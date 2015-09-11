#include "Config.h"

#include <QSettings>

Config& Config::instance()
{
	static Config effects;
	return effects;
}

Config::Config()
{
	load();
}

bool Config::isEmpty()
{
	return m_isEmpty;
}

void Config::load()
{
	QSettings settings("SAH.ini", QSettings::IniFormat);

	m_isEmpty = !settings.contains("useModOrganizer");

	dataFolder = settings.value("skyrimDataFolder").toString();
	savesFolder = settings.value("savesFolder").toString();
	pluginsListPath = settings.value("pluginsListFile").toString();
	modOrganizerPath = settings.value("modOrganizerPath").toString();
	selectedSaveName = settings.value("selectedSaveName").toString();

	useModOrganizer = settings.value("useModOrganizer").toBool();
	loadMostRecentSave = settings.value("loadMostRecentSave").toBool();
}

void Config::save()
{
	QSettings settings("SAH.ini", QSettings::IniFormat);

	settings.setValue("skyrimDataFolder", dataFolder);
	settings.setValue("savesFolder", savesFolder);
	settings.setValue("pluginsListFile", pluginsListPath);
	settings.setValue("modOrganizerPath", modOrganizerPath);
	settings.setValue("selectedSaveName", selectedSaveName);

	settings.setValue("useModOrganizer", useModOrganizer);
	settings.setValue("loadMostRecentSave", loadMostRecentSave);
}
