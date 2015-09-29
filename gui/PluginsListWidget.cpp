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

#include "PluginsListWidget.h"
#include "Config.h"

PluginsListModel::PluginsListModel(Config& config, QObject* parent)
	: QAbstractTableModel(parent)
	, m_config(config)
{
}

int PluginsListModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_config.plugins.size();
}

int PluginsListModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 2;
}

QVariant PluginsListModel::data(const QModelIndex& index, int role) const
{
	switch (role)
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
	{
		const auto& plugin = m_config.plugins[index.row()];
		switch (index.column())
		{
		case 0: return plugin.name;
		case 1: return plugin.nbIngredients;
		}
		return QVariant();
	}
	}
	return QVariant();
}

QVariant PluginsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0: return QString("name");
		case 1: return QString("# ingredients");
		}
	}
	return QVariant();
}

void PluginsListModel::beginReset()
{
	beginResetModel();
}

void PluginsListModel::endReset()
{
	endResetModel();
}

//****************************************************************************//

PluginsListWidget::PluginsListWidget(Config& config, QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	m_view = new QTableView(this);
	m_view->setSortingEnabled(true);
	m_model = new PluginsListModel(config, this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(m_model);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	proxyModel->setDynamicSortFilter(true);
	m_view->setModel(m_model);
	m_view->sortByColumn(0, Qt::AscendingOrder);
	m_view->horizontalHeader()->resizeSection(0, 250);
	m_view->setSelectionMode(QAbstractItemView::NoSelection);
	vLayout->addWidget(m_view);

	setLayout(vLayout);
}

void PluginsListWidget::beginReset()
{
	m_model->beginReset();
}

void PluginsListWidget::endReset()
{
	m_model->endReset();
}
