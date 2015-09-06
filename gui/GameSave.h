#pragma once

#include <QString>
#include <QVector>
#include <QPixmap>

class GameSave
{
public:
	static GameSave& instance();

	void loadMostRecentSave();

	void load(QString fileName);
	bool loaded();

	QPixmap screenshot();

protected:
	GameSave();

	bool m_loaded = false;
	QPixmap m_screenshot;
};

//****************************************************************************//

inline bool GameSave::loaded()
{ return m_loaded; }

inline QPixmap GameSave::screenshot()
{ return m_screenshot; }