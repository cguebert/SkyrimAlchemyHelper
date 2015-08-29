#include <QtWidgets>

#include "EffectsListWidget.h"
#include "EffectsListModel.h"
#include "EffectsList.h"

EffectsListWidget::EffectsListWidget(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto tableView = new QTableView(this);
	tableView->setSortingEnabled(true);
	auto sourceModel = new EffectsListModel(this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(sourceModel);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
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
