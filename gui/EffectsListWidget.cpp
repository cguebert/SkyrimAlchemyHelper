#include <QtWidgets>

#include "EffectsListWidget.h"
#include "EffectsListModel.h"
#include "EffectsList.h"

EffectsListWidget::EffectsListWidget(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto m_view = new QTableView(this);
	m_view->setSortingEnabled(true);
	m_model = new EffectsListModel(this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(m_model);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_view->setModel(proxyModel);
	m_view->sortByColumn(0, Qt::AscendingOrder);
	m_view->horizontalHeader()->resizeSection(0, 250);
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_view->setSelectionMode(QAbstractItemView::SingleSelection);
	m_view->hideColumn(1);
	vLayout->addWidget(m_view);

	setLayout(vLayout);
}

void EffectsListWidget::beginReset()
{
	m_model->beginReset();
}

void EffectsListWidget::endReset()
{
	m_model->endReset();
}
