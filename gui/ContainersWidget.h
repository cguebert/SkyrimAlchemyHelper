#pragma once
#include <QFrame>

class GameSave;
class QLabel;
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
	QString getContainerLabel(quint32 id);

	GameSave& m_gameSave;
	std::vector<QPushButton*> m_toggleButtons;
	std::vector<QWidget*> m_inventoryWidgets;
	std::vector<QLabel*> m_idLabels;
};
