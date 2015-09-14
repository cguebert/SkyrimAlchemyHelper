#include "Settings.h"

#include <QSettings>

Settings& Settings::instance()
{
	static Settings effects;
	return effects;
}

Settings::Settings()
{
	load();
}

bool Settings::isEmpty()
{
	return m_isEmpty;
}

void Settings::load()
{
	QSettings settings("SAH.ini", QSettings::IniFormat);

	m_isEmpty = !settings.contains("useModOrganizer");

	dataFolder = settings.value("skyrimDataFolder").toString();
	savesFolder = settings.value("savesFolder").toString();
	pluginsListPath = settings.value("pluginsListFile").toString();
	modOrganizerPath = settings.value("modOrganizerPath").toString();
	selectedSavePath = settings.value("selectedSavePath").toString();

	useModOrganizer = settings.value("useModOrganizer").toBool();
	loadMostRecentSave = settings.value("loadMostRecentSave").toBool();

	maxValidIngredientCount = settings.value("maxValidIngredientCount", 1000).toInt();
	minValidNbIngredients = settings.value("minValidNbIngredients", 5).toInt();
	minTotalIngredientsCount = settings.value("minTotalIngredientsCount", 25).toInt();
}

void Settings::save()
{
	QSettings settings("SAH.ini", QSettings::IniFormat);

	settings.setValue("skyrimDataFolder", dataFolder);
	settings.setValue("savesFolder", savesFolder);
	settings.setValue("pluginsListFile", pluginsListPath);
	settings.setValue("modOrganizerPath", modOrganizerPath);
	settings.setValue("selectedSavePath", selectedSavePath);

	settings.setValue("useModOrganizer", useModOrganizer);
	settings.setValue("loadMostRecentSave", loadMostRecentSave);

	settings.setValue("maxValidIngredientCount", maxValidIngredientCount);
	settings.setValue("minValidNbIngredients", minValidNbIngredients);
	settings.setValue("minTotalIngredientsCount", minTotalIngredientsCount);
}
