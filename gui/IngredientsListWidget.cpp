#include <QtWidgets>

#include "IngredientsListWidget.h"
#include "IngredientsListModel.h"

IngredientsListWidget::IngredientsListWidget(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto m_view = new QTableView(this);
	m_view->setSortingEnabled(true);
	m_model = new IngredientsListModel(this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(m_model);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_view->setModel(proxyModel);
	m_view->sortByColumn(0, Qt::AscendingOrder);
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_view->setSelectionMode(QAbstractItemView::SingleSelection);
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
