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
	vLayout->addWidget(tableView);

	connect(tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
			this, SLOT(currentChanged(QModelIndex,QModelIndex)));

	m_addButton = new QPushButton(tr("Add"), this);
	connect(m_addButton, SIGNAL(clicked()), this, SLOT(addRow()));
	m_removeButton = new QPushButton(tr("Remove"), this);
	m_removeButton->setEnabled(false);
	connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeRow()));
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(m_addButton);
	buttonsLayout->addWidget(m_removeButton);
	buttonsLayout->addStretch();
	vLayout->addLayout(buttonsLayout);

	setLayout(vLayout);

	m_view = tableView;
	m_model = proxyModel;
}

void EffectsListWidget::currentChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
	static EffectsList& effects = EffectsList::GetInstance();
	auto mapped = m_model->mapToSource(current);
	m_removeButton->setEnabled(current.isValid() && !effects.effect(mapped.row()).nbIngredients);
}

void EffectsListWidget::addRow()
{
	int id = m_model->rowCount();
	if(m_model->insertRows(id, 1))
	{
		auto index = m_model->sourceModel()->index(id, 0);
		auto mapped = m_model->mapFromSource(index);
		m_view->selectRow(mapped.row());
		m_view->setFocus();
	}
}

void EffectsListWidget::removeRow()
{
	auto indexes = m_view->selectionModel()->selectedRows();
	if(!indexes.empty())
		m_model->removeRows(indexes.front().row(), 1);
}
