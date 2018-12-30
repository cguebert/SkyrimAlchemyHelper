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
#include "Settings.h"

#include <QSettings>

namespace
{
QStringList gameNames()
{
	return QStringList() << "Skyrim"
						 << "Skyrim Special Edition"
						 << "Skyrim VR";
}

QString noSpace(QString str)
{
	return str.replace(" ", "");
}
} // namespace

Settings& Settings::instance()
{
	static Settings effects;
	return effects;
}

Settings::Settings()
{
	load();
}

bool Settings::isEmpty() const
{
	return m_isEmpty;
}

void Settings::clearEmptyFlag()
{
	m_isEmpty = false;
}

void Settings::load()
{
	QSettings settings("SAH.ini", QSettings::IniFormat);

	m_isEmpty = !settings.contains("game");

	gameName = settings.value("game", "Skyrim").toString();
	language = settings.value("language", "english").toString();

	const QStringList groups = settings.childGroups();
	for (const auto name : gameNames())
	{
		const QString groupName = noSpace(name);
		if (!groups.contains(groupName))
			continue;

		auto& specific = gameSpecific[name];
		settings.beginGroup(groupName);
		specific.dataFolder = settings.value("skyrimDataFolder").toString();
		specific.savesFolder = settings.value("savesFolder").toString();
		specific.pluginsListPath = settings.value("pluginsListFile").toString();
		specific.modOrganizerPath = settings.value("modOrganizerPath").toString();
		specific.selectedSavePath = settings.value("selectedSavePath").toString();
		specific.useModOrganizer = settings.value("useModOrganizer").toBool();
		settings.endGroup();
	}

	loadMostRecentSave = settings.value("loadMostRecentSave", true).toBool();
	playerOnly = settings.value("playerOnly", false).toBool();
	getContainersInfo = settings.value("getContainersInfo", true).toBool();
	sameCellAsPlayer = settings.value("sameCellAsPlayer", false).toBool();
	interiorCellsOnly = settings.value("interiorCellsOnly", false).toBool();

	maxValidIngredientCount = settings.value("maxValidIngredientCount", 1000).toInt();
	minValidNbIngredients = settings.value("minValidNbIngredients", 5).toInt();
	minTotalIngredientsCount = settings.value("minTotalIngredientsCount", 25).toInt();
}

void Settings::save() const
{
	QSettings settings("SAH.ini", QSettings::IniFormat);

	settings.setValue("game", gameName);
	settings.setValue("language", language);

	for (const auto name : gameNames())
	{
		if (!gameSpecific.count(name))
			continue;

		const auto& specific = gameSpecific.at(name);
		settings.beginGroup(noSpace(name));
		settings.setValue("skyrimDataFolder", specific.dataFolder);
		settings.setValue("savesFolder", specific.savesFolder);
		settings.setValue("pluginsListFile", specific.pluginsListPath);
		settings.setValue("modOrganizerPath", specific.modOrganizerPath);
		settings.setValue("selectedSavePath", specific.selectedSavePath);
		settings.setValue("useModOrganizer", specific.useModOrganizer);
		settings.endGroup();
	}

	settings.setValue("loadMostRecentSave", loadMostRecentSave);
	settings.setValue("playerOnly", playerOnly);
	settings.setValue("getContainersInfo", getContainersInfo);
	settings.setValue("sameCellAsPlayer", sameCellAsPlayer);
	settings.setValue("interiorCellsOnly", interiorCellsOnly);

	settings.setValue("maxValidIngredientCount", maxValidIngredientCount);
	settings.setValue("minValidNbIngredients", minValidNbIngredients);
	settings.setValue("minTotalIngredientsCount", minTotalIngredientsCount);
}

Settings::GameSpecific& Settings::currentGame()
{
	return gameSpecific[gameName];
}
