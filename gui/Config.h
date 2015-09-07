#pragma once

#include <QString>
#include <QVector>

class Config
{
public:
	static Config& instance();

	QString dataFolder, savesFolder, pluginsListPath, modOrganizerPath;
	bool useModOrganizer;

	bool empty();
	void load();
	void save();

protected:
	Config();

	bool m_empty;
};
