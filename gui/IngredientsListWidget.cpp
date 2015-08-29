#include <QtWidgets>

#include "IngredientsListWidget.h"
#include "IngredientsListModel.h"

IngredientsListWidget::IngredientsListWidget(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto tableView = new QTableView(this);
	tableView->setSortingEnabled(true);
	auto sourceModel = new IngredientsListModel(this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(sourceModel);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	tableView->setModel(proxyModel);
	tableView->sortByColumn(0, Qt::AscendingOrder);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView->setSelectionMode(QAbstractItemView::SingleSelection);
//	tableView->hideColumn(1);
	for(int i = 0; i < 4; ++i)
	{
		tableView->hideColumn(i * 3 + 3);
		tableView->hideColumn(i * 3 + 4);
	}
	tableView->horizontalHeader()->resizeSection(0, 250);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	vLayout->addWidget(tableView);

	setLayout(vLayout);

	m_view = tableView;
	m_model = proxyModel;
}
