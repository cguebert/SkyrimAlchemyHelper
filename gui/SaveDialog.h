#pragma once

#include <QDialog>

#include "GameSave.h"

class QWidget;
class InventoryWidget;

class SaveDialog : public QDialog
{
	Q_OBJECT

public:
	SaveDialog(QWidget *parent = 0);

	QSize sizeHint() const;
	bool modified() const;

public slots:
	void loadSave();
	void copySave();

protected:
	void refreshInformation();

	QWidget* m_saveInfoContainer;
	InventoryWidget* m_inventoryWidget;
	GameSave m_gameSave;
	bool m_modified = false;
};
