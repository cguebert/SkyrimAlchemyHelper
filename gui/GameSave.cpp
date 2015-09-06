#include "GameSave.h"

#include <saveParser/Save.h>

GameSave& GameSave::instance()
{
	static GameSave save;
	return save;
}

GameSave::GameSave()
{
	loadMostRecentSave();
}

void GameSave::load(QString fileName)
{
	m_screenshot = QPixmap();

	Save save;
	m_loaded = save.parse(fileName.toStdString());
	if (!m_loaded)
		return;

	auto screenshot = save.screenshot();
	auto img = QImage(screenshot.data.data(), screenshot.width, screenshot.height, QImage::Format_RGB32);
	m_screenshot.convertFromImage(img);
}

void GameSave::loadMostRecentSave()
{
	load("../Saves/quicksave.ess");
}