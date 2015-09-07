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

bool Config::empty()
{
	return m_empty;
}

void Config::load()
{
	QSettings settings;

	m_empty = !settings.contains("useModOrganizer");

	useModOrganizer = settings.value("useModOrganizer").toBool();
	dataFolder = settings.value("skyrimDataFolder").toString();
	savesFolder = settings.value("savesFolder").toString();
	pluginsListPath = settings.value("pluginsListFile").toString();
	modOrganizerPath = settings.value("modOrganizerPath").toString();
}

void Config::save()
{
	QSettings settings;

	settings.setValue("useModOrganizer", useModOrganizer);
	settings.setValue("skyrimDataFolder", dataFolder);
	settings.setValue("savesFolder", savesFolder);
	settings.setValue("pluginsListFile", pluginsListPath);
	settings.setValue("modOrganizerPath", modOrganizerPath);
}
