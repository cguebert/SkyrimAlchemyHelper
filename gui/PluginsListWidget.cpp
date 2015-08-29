#include <QtWidgets>

#include "PluginsListWidget.h"
#include "PluginsListModel.h"
#include "PluginsList.h"

PluginsListWidget::PluginsListWidget(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto tableView = new QTableView(this);
	tableView->setSortingEnabled(true);
	auto sourceModel = new PluginsListModel(this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(sourceModel);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	proxyModel->setDynamicSortFilter(true);
	tableView->setModel(proxyModel);
	tableView->sortByColumn(0, Qt::AscendingOrder);
	tableView->horizontalHeader()->resizeSection(0, 250);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	tableView->hideColumn(1);
	vLayout->addWidget(tableView);

	setLayout(vLayout);

	m_view = tableView;
	m_model = proxyModel;
}
