#include <QtWidgets>

#include "PluginsListWidget.h"
#include "PluginsListModel.h"
#include "PluginsList.h"

PluginsListWidget::PluginsListWidget(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	m_view = new QTableView(this);
	m_view->setSortingEnabled(true);
	m_model = new PluginsListModel(this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(m_model);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	proxyModel->setDynamicSortFilter(true);
	m_view->setModel(m_model);
	m_view->sortByColumn(0, Qt::AscendingOrder);
	m_view->horizontalHeader()->resizeSection(0, 250);
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_view->setSelectionMode(QAbstractItemView::SingleSelection);
	m_view->hideColumn(1);
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
