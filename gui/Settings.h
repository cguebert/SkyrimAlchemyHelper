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
		selectedSavePath;	// File path of the game save to load

	bool useModOrganizer = false,	// Is Mod Organizer used to separate mods ?
		loadMostRecentSave = true;	// Always load the most recent save instead of a user chosen one

	bool isEmpty();
	void load();
	void save();

protected:
	Settings();

	bool m_isEmpty = false;
};
