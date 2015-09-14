#include <QtWidgets>

#include "InventoryWidget.h"
#include "Config.h"

InventoryModel::InventoryModel(const std::vector<std::pair<int, int>>& inventory, QObject* parent)
	: QAbstractTableModel(parent)
	, m_inventory(inventory)
{
}

int InventoryModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_inventory.size();
}

int InventoryModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 2;
}

QVariant InventoryModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const auto& item = m_inventory[index.row()];
		switch (index.column())
		{
		case 0: return Config::main().ingredients[item.first].name;
		case 1: return item.second;
		}
		return QVariant();
	}
	return QVariant();
}

QVariant InventoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0: return QString("ingredient");
		case 1: return QString("count");
		}
	}
	return QVariant();
}

void InventoryModel::beginReset()
{
	beginResetModel();
}

void InventoryModel::endReset()
{
	endResetModel();
}

//****************************************************************************//

InventoryWidget::InventoryWidget(const std::vector<std::pair<int, int>>& inventory, QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto m_view = new QTableView(this);
	m_view->setSortingEnabled(true);
	m_model = new InventoryModel(inventory, this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(m_model);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_view->setModel(proxyModel);
	m_view->sortByColumn(0, Qt::AscendingOrder);
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_view->setSelectionMode(QAbstractItemView::SingleSelection);
	m_view->horizontalHeader()->resizeSection(0, 250);
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	vLayout->addWidget(m_view);

	setLayout(vLayout);
}

void InventoryWidget::beginReset()
{
	m_model->beginReset();
}

void InventoryWidget::endReset()
{
	m_model->endReset();
}
