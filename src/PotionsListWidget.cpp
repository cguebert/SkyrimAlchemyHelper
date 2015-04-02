#include <QtWidgets>

#include "PotionsListWidget.h"
#include "PotionsListModel.h"
#include "PotionsList.h"

PotionsListWidget::PotionsListWidget(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto tableView = new QTableView(this);
	tableView->setModel(new PotionsListModel(this));
	tableView->setWordWrap(true);
	tableView->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
	vLayout->addWidget(tableView);

	setLayout(vLayout);
}
