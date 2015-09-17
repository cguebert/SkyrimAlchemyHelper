#pragma once

#include <QString>
#include <QVector>

class Settings
{
public:
	static Settings& instance();

	QString dataFolder,		// Skyrim data folder ([...]/Skyrim/Data)
		savesFolder,		// Saves folder ([User documents]/My Games/Skyrim/Saves)
		pluginsListPath,	// Path to the file containing the plugins list ([User Local App Data]/Skyrim/plugins.txt)
		modOrganizerPath,	// Path to ModOrganizer (C:/Program files/Mod Organizer/Mor Organizer.exe)
		selectedSavePath,	// File path of the game save to load
		language;			// When parsing mods, try to use the strings table for this language

	bool useModOrganizer = false,	// Is Mod Organizer used to separate mods ?
		loadMostRecentSave = true,	// If true, always load the most recent save instead of a user chosen one
		playerOnly = false,			// If true, only parse the player and ignore all containers
		getContainersInfo = true,	// If true, extract containers information from the mods
		sameCellAsPlayer = false;	// If true, only keept containers that are in the same cell as the player

	int maxValidIngredientCount = 1000, // When parsing a save, an ingredient count higher than this is considered invalid (if 0, accept everything)
		minValidNbIngredients = 5, // When parsing a save, ignore containers and the player inventory if there is less than this number of different ingredients (if 0, accept everything)
		minTotalIngredientsCount = 25; // When parsing a save, ignore containers that have less than this total number of ingredients

	bool isEmpty();
	void load();
	void save();

protected:
	Settings();

	bool m_isEmpty = false;
};
