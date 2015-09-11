#include <QtWidgets>

#include "PluginsListWidget.h"
#include "PluginsList.h"

PluginsListModel::PluginsListModel(PluginsList& pluginsList, QObject* parent)
	: QAbstractTableModel(parent)
	, m_pluginsList(pluginsList)
{

}

int PluginsListModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_pluginsList.size();
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
		const auto& plugin = m_pluginsList.plugins()[index.row()];
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

PluginsListWidget::PluginsListWidget(PluginsList& pluginsList, QWidget* parent) 
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	m_view = new QTableView(this);
	m_view->setSortingEnabled(true);
	m_model = new PluginsListModel(pluginsList, this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(m_model);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	proxyModel->setDynamicSortFilter(true);
	m_view->setModel(m_model);
	m_view->sortByColumn(0, Qt::AscendingOrder);
	m_view->horizontalHeader()->resizeSection(0, 250);
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_view->setSelectionMode(QAbstractItemView::SingleSelection);
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
