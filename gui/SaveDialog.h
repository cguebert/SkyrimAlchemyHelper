#pragma once

#include <QDialog>
#include <QThread>

#include "GameSave.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QWidget;

class InventoryWidget;
class KnownIngredientsWidget;
class ContainersWidget;

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
	void saveSelected(int index);
	void setLoadMostRecent(int state);
	void getContainersNamesChanged(int state);
	void containersUpdated();
	void updateIngredientsCount();

protected:
	void refreshInformation();
	void refreshContainersNames();
	void selectCurrentSave();

	QWidget* m_saveInfoContainer;
	InventoryWidget* m_inventoryWidget;
	KnownIngredientsWidget* m_knownIngredientsWidget;
	ContainersWidget* m_containersWidget;
	GameSave m_gameSave;
	QFileInfoList m_savesList;
	QStringList m_savesNames;
	bool m_modified = false, m_loadMostRecent;
	QString m_selectedSavePath;
	QComboBox *m_saveComboBox;
	QLineEdit *m_maxValidIngredientCountEdit, 
		*m_minValidNbIngredientsEdit, 
		*m_minTotalIngredientsCountEdit;
	QCheckBox *m_playerOnlyCheckBox,
		*m_getContainersNamesCheckBox,
		*m_interiorCellsOnlyCheckBox,
		*m_sameCellAsPlayerCheckBox;
	QLabel *m_sameCellAsPlayerLabel,
		*m_interiorCellsOnlyLabel;
};

class ContainersWorkerThread : public QThread
{
Q_OBJECT
public:
	ContainersWorkerThread(std::vector<uint32_t> ids, QObject* parent = nullptr)
		: m_ids(ids), QThread(parent) {}

	void run() override;

signals:
	void resultReady();

protected:
	std::vector<uint32_t> m_ids;
};
