#pragma once
#include <QFrame>

class GameSave;

class ContainersWidget : public QFrame
{
	Q_OBJECT
public:
	explicit ContainersWidget(GameSave& gameSave, QWidget* parent = nullptr);

public slots:
	void refreshList();

protected:
	GameSave& m_gameSave;
};
