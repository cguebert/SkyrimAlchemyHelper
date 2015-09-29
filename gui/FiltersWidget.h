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

enum class FilterActionType
{ addFilterContains, addFilterDoesNotContain, RemoveFilter };

class FlowLayout;
class QCheckBox;
class QComboBox;

class FiltersWidget : public QWidget
{
	Q_OBJECT
public:
	explicit FiltersWidget(QWidget* parent = nullptr);

	void clear();
	QSize sizeHint() const;

signals:
	void modified();
	void effectFilterRemoved(int);
	void ingredientFilterRemoved(int);

public slots:
	void effectFilterAction(FilterActionType action, int id);
	void ingredientFilterAction(FilterActionType action, int id);
	void removeEffect();
	void removeIngredient();
	void updatePotionsListFilters();

protected:
	struct FilterItem
	{
		FilterItem() {}
		FilterItem(FilterActionType a, int i, QWidget* w) : actionType(a), id(i), widget(w) {}

		FilterActionType actionType;
		int id;
		QWidget* widget;
	};

	bool updateExisting(std::vector<FilterItem>& list, FlowLayout *layout, FilterActionType action, int id);
	void removeWidget(FlowLayout *layout, QWidget* widget);

	FlowLayout *m_effectsLayout, *m_ingredientsLayout;
	std::vector<FilterItem> m_effectsFilters, m_ingredientsFilters;
	QCheckBox *m_inventoryCheckBox;
	QComboBox *m_ingredientsCountComboBox, *m_purityComboBox;
};
