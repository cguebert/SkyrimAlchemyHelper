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

#include <QWidget>

#include <vector>
#include <cstdint>

#include "FiltersWidget.h"

class QPushButton;
class QWidget;

class SelectorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SelectorWidget(QWidget* parent = nullptr);

	void setItems(QStringList items, QStringList tooltips);

public slots:
	void modifyFilter();
	void filterRemoved(int);

protected:
	virtual bool filterAction(FilterActionType actionType, int id) = 0;

	QStringList m_items, m_tooltips;

	struct ItemBox
	{
		QWidget *widget;
		QPushButton *addContainsButton, *addDoesNotContainButton, *removeButton;
	};

	using ItemBoxes = std::vector<ItemBox>;
	ItemBoxes m_itemBoxes;
};

//****************************************************************************//

class EffectsSelector : public SelectorWidget
{
	Q_OBJECT
public:
	explicit EffectsSelector(QWidget* parent = nullptr);

signals:
	void effectFilterAction(FilterActionType action, int id);

	public slots:
	void updateList();

protected:
	bool filterAction(FilterActionType action, int id) override;
};

//****************************************************************************//

class IngredientsSelector : public SelectorWidget
{
	Q_OBJECT
public:
	explicit IngredientsSelector(QWidget* parent = nullptr);

signals:
	void ingredientFilterAction(FilterActionType action, int id);

	public slots:
	void updateList();

protected:
	bool filterAction(FilterActionType action, int id) override;
};
