#pragma once
#include <QFrame>

class GameSave;
class QCheckBox;
class QLabel;
class QPushButton;

class ContainersWidget : public QFrame
{
	Q_OBJECT
public:
	explicit ContainersWidget(GameSave& gameSave, QWidget* parent = nullptr);

	void clear();
	void refreshList();
	void updateIdLabels();
	void updateCheckBoxes();

signals:
	void containersStatusUpdated();

public slots:
	void toggleInventoryWidget();
	void toggleContainerStatus();

protected:
	QString getContainerLabel(quint32 id);

	GameSave& m_gameSave;

	struct ContainerWidgets
	{
		quint32 id;
		QCheckBox* activeCheckBox;
		QPushButton* toggleButton;
		QLabel* idLabel;
		QWidget* inventoryWidget;
	};
	using ContainerWidgetsList = std::vector<ContainerWidgets>;
	ContainerWidgetsList m_containersWidgets;
};
