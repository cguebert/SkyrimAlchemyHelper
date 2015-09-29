/******************************************************************************
*                            Skyrim Alchemy Helper                            *
*******************************************************************************
*                                                                             *
* Copyright (C) 2015 Christophe Guebert                                       *
*                                                                             *
* This program is free software; you can redistribute it and/or modify        *
* it under the terms of the GNU General Public License as published by        *
* the Free Software Foundation; either version 2 of the License, or           *
* (at your option) any later version.                                         *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc.,     *
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                 *
*******************************************************************************
*                        Skyrim Alchemy Helper :: Gui                         *
*                                                                             *
*                  Contact: christophe.guebert+SAH@gmail.com                  *
******************************************************************************/
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
