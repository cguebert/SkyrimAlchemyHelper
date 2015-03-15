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
	tableView->hideColumn(1);
	for(int i = 0; i < 4; ++i)
	{
		tableView->hideColumn(i * 3 + 3);
		tableView->hideColumn(i * 3 + 4);
	}
	tableView->horizontalHeader()->resizeSection(0, 250);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	vLayout->addWidget(tableView);

	connect(tableView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
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

void IngredientsListWidget::currentChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
	m_removeButton->setEnabled(current.isValid());
}

void IngredientsListWidget::addRow()
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

void IngredientsListWidget::removeRow()
{
	auto indexes = m_view->selectionModel()->selectedRows();
	if(!indexes.empty())
		m_model->removeRows(indexes.front().row(), 1);
}
