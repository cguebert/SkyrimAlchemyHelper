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
#include <QtWidgets>

#include "IngredientsListWidget.h"
#include "Config.h"

IngredientsListModel::IngredientsListModel(Config& config, QObject* parent)
	: QAbstractTableModel(parent)
	, m_config(config)
{
}

int IngredientsListModel::rowCount(const QModelIndex& /*parent*/) const
{
	return static_cast<int>(m_config.ingredients.size());
}

int IngredientsListModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 14;
}

QVariant IngredientsListModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const auto& ingredient = m_config.ingredients[index.row()];
		switch (index.column())
		{
		case 0: return ingredient.name;
		case 1:
			return ingredient.pluginId == -1 ? "" : m_config.plugins[ingredient.pluginId].name;
		case 2:
		{
			const auto id = ingredient.sortedEffects[0].effectId;
			return id == -1 ? "" : m_config.effects[id].name;
		}
		case 3: return ingredient.sortedEffects[0].magnitude;
		case 4: return ingredient.sortedEffects[0].duration;
		case 5:
		{
			const auto id = ingredient.sortedEffects[1].effectId;
			return id == -1 ? "" : m_config.effects[id].name;
		}
		case 6: return ingredient.sortedEffects[1].magnitude;
		case 7: return ingredient.sortedEffects[1].duration;
		case 8:
		{
			const auto id = ingredient.sortedEffects[2].effectId;
			return id == -1 ? "" : m_config.effects[id].name;
		}
		case 9: return ingredient.sortedEffects[2].magnitude;
		case 10: return ingredient.sortedEffects[2].duration;
		case 11:
		{
			const auto id = ingredient.sortedEffects[3].effectId;
			return id == -1 ? "" : m_config.effects[id].name;
		}
		case 12: return ingredient.sortedEffects[3].magnitude;
		case 13: return ingredient.sortedEffects[3].duration;
		}
		return QVariant();
	}
	return QVariant();
}

QVariant IngredientsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0: return QString("name");
		case 1: return QString("plugin");
		case 2: return QString("ingredient 1");
		case 3: return QString("magnitude 1");
		case 4: return QString("duration 1");
		case 5: return QString("ingredient 2");
		case 6: return QString("magnitude 2");
		case 7: return QString("duration 2");
		case 8: return QString("ingredient 3");
		case 9: return QString("magnitude 3");
		case 10: return QString("duration 3");
		case 11: return QString("ingredient 4");
		case 12: return QString("magnitude 4");
		case 13: return QString("duration 4");
		}
	}
	return QVariant();
}

void IngredientsListModel::beginReset()
{
	beginResetModel();
}

void IngredientsListModel::endReset()
{
	endResetModel();
}

//****************************************************************************//

IngredientsListWidget::IngredientsListWidget(Config& config, QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto m_view = new QTableView(this);
	m_view->setSortingEnabled(true);
	m_model = new IngredientsListModel(config, this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(m_model);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_view->setModel(proxyModel);
	m_view->sortByColumn(0, Qt::AscendingOrder);
	m_view->setSelectionMode(QAbstractItemView::NoSelection);
//	m_view->hideColumn(1);
	for(int i = 0; i < 4; ++i)
	{
		m_view->hideColumn(i * 3 + 3);
		m_view->hideColumn(i * 3 + 4);
	}
	m_view->horizontalHeader()->resizeSection(0, 250);
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	vLayout->addWidget(m_view);

	setLayout(vLayout);
}

void IngredientsListWidget::beginReset()
{
	m_model->beginReset();
}

void IngredientsListWidget::endReset()
{
	m_model->endReset();
}
