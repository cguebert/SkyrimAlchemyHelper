#include <QtWidgets>

#include "SelectorWidget.h"

SelectorWidget::SelectorWidget(QWidget* parent) 
	: QWidget(parent)
{
}

void SelectorWidget::setItems(QStringList items, QStringList tooltips)
{
	m_items = items;
	m_tooltips = tooltips;

	auto layout = this->layout();
	if (layout)
		QWidget().setLayout(layout); // Clean up

	auto mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(5, 5, 5, 5);
	mainLayout->setSpacing(0);

	auto addFilterContainsIcon = QIcon("://icons/Add.png");
	auto addFilterDoesNotContainIcon = QIcon("://icons/Remove.png");
	auto removeFilterIcon = QIcon("://icons/Delete.png");

	int addFilterContainsMask = static_cast<int>(FilterActionType::addFilterContains) << 16;
	int addFilterDoesNotContainMask = static_cast<int>(FilterActionType::addFilterDoesNotContain) << 16;
	int removeFilterMask = static_cast<int>(FilterActionType::RemoveFilter) << 16;

	const int buttonSize = 24;

	for (int i = 0, nb = items.size(); i < nb; ++i)
	{
		auto boxLayout = new QHBoxLayout;
		boxLayout->setContentsMargins(0, 0, 0, 0);
		boxLayout->setSpacing(2);

		auto addFilterContainsAction = new QAction(boxLayout);
		addFilterContainsAction->setData(QVariant(i + addFilterContainsMask));
		auto addFilterContainsButton = new QPushButton(addFilterContainsIcon, "");
		addFilterContainsButton->setMaximumSize(buttonSize, buttonSize);
		addFilterContainsButton->setFlat(true);
		connect(addFilterContainsButton, SIGNAL(clicked(bool)), addFilterContainsAction, SLOT(trigger()));
		connect(addFilterContainsAction, SIGNAL(triggered(bool)), this, SLOT(modifyFilter()));
		boxLayout->addWidget(addFilterContainsButton);

		auto addFilterDoesNotContainAction = new QAction(boxLayout);
		addFilterDoesNotContainAction->setData(QVariant(i + addFilterDoesNotContainMask));
		auto addFilterDoesNotContainButton = new QPushButton(addFilterDoesNotContainIcon, "");
		addFilterDoesNotContainButton->setMaximumSize(buttonSize, buttonSize);
		addFilterDoesNotContainButton->setFlat(true);
		connect(addFilterDoesNotContainButton, SIGNAL(clicked(bool)), addFilterDoesNotContainAction, SLOT(trigger()));
		connect(addFilterDoesNotContainAction, SIGNAL(triggered(bool)), this, SLOT(modifyFilter()));
		boxLayout->addWidget(addFilterDoesNotContainButton);

	/*	auto removeFilterAction = new QAction(boxLayout);
		removeFilterAction->setData(QVariant(i + removeFilterMask));
		auto removeFilterButton = new QPushButton(removeFilterIcon, "");
		removeFilterButton->setMaximumSize(buttonSize, buttonSize);
		removeFilterButton->setFlat(true);
		connect(removeFilterButton, SIGNAL(clicked(bool)), removeFilterAction, SLOT(trigger()));
		connect(removeFilterAction, SIGNAL(triggered(bool)), this, SLOT(modifyFilter()));
		boxLayout->addWidget(removeFilterButton);
		*/

		auto label = new QLabel(m_items[i]);
		label->setToolTip(m_tooltips[i]);
		boxLayout->addWidget(label);

		mainLayout->addLayout(boxLayout);
	}
}

void SelectorWidget::modifyFilter()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
	{
		bool ok = false;
		int val = action->data().toInt(&ok);
		if (ok)
		{
			int typeVal = val >> 16;
			auto type = static_cast<FilterActionType>(typeVal);
			int id = val & 0xFFFF;
			filterAction(type, id);
		}
	}
}
