#pragma once
#include <QFrame>

class GameSave;
class QPushButton;

class ContainersWidget : public QFrame
{
	Q_OBJECT
public:
	explicit ContainersWidget(GameSave& gameSave, QWidget* parent = nullptr);

	void clear();
	void refreshList();

public slots:
	void toggleInventoryWidget();

protected:
	GameSave& m_gameSave;
	std::vector<QPushButton*> m_toggleButtons;
	std::vector<QWidget*> m_inventoryWidgets;
};
