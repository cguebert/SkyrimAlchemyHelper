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

#include <QMainWindow>

class EffectsSelector;
class IngredientsSelector;
class FiltersWidget;
class PotionsListWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);

	QSize sizeHint() const;

signals:
	void mainWindowShown();

public slots:
	void editConfig();
	void afterLaunch();
	void gameSaveInformation();
	void discoverEffects();
	void about();

protected:
	void showEvent(QShowEvent* event);
	void closeEvent(QCloseEvent* event);

	void readSettings();
	void writeSettings();
	
	EffectsSelector *m_effectsSelector;
	IngredientsSelector *m_ingredientsSelector;
	FiltersWidget *m_filtersWidget;
	PotionsListWidget *m_potionsWidget;
};
