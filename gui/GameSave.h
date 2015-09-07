#pragma once

#include <QString>
#include <QPixmap>

#include <array>
#include <vector>

class GameSave
{
public:
	static GameSave& instance();

	void loadSaveFromConfig();

	void load(QString fileName);
	bool isLoaded() const;

	QPixmap screenshot() const;

	using KnownIngredient = std::pair<int, std::array<bool, 4>>;
	using KnownIngredients = std::vector<KnownIngredient>;
	const KnownIngredients& knownIngredients() const;

protected:
	GameSave();

	bool m_isLoaded = false;
	QPixmap m_screenshot;
	KnownIngredients m_knownIngredients;
};

//****************************************************************************//

inline bool GameSave::isLoaded() const
{ return m_isLoaded; }

inline QPixmap GameSave::screenshot() const
{ return m_screenshot; }

inline const GameSave::KnownIngredients& GameSave::knownIngredients() const
{ return m_knownIngredients; }