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
	QString getContainerLabel(size_t id);

	GameSave& m_gameSave;

	struct ContainerWidgets
	{
		size_t id;
		QCheckBox* activeCheckBox;
		QPushButton* toggleButton;
		QLabel* idLabel;
		QWidget* inventoryWidget;
	};
	using ContainerWidgetsList = std::vector<ContainerWidgets>;
	ContainerWidgetsList m_containersWidgets;
};
