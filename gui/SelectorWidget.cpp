#include <QtWidgets>

#include "SelectorWidget.h"
#include "Config.h"

SelectorWidget::SelectorWidget(QWidget* parent) 
	: QWidget(parent)
{
}

void SelectorWidget::setItems(QStringList items, QStringList tooltips)
{
	m_itemBoxes.clear();
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
		auto boxWidget = new QWidget;
		auto boxLayout = new QHBoxLayout;
		boxLayout->setContentsMargins(0, 0, 0, 0);
		boxLayout->setSpacing(2);

		auto addFilterContainsAction = new QAction(boxWidget);
		addFilterContainsAction->setData(QVariant(i + addFilterContainsMask));
		auto addFilterContainsButton = new QPushButton(addFilterContainsIcon, "", boxWidget);
		addFilterContainsButton->setMaximumSize(buttonSize, buttonSize);
		addFilterContainsButton->setFlat(true);
		addFilterContainsButton->setToolTip(tr("Only show potions that contain this item"));
		connect(addFilterContainsButton, SIGNAL(clicked(bool)), addFilterContainsAction, SLOT(trigger()));
		connect(addFilterContainsAction, SIGNAL(triggered(bool)), this, SLOT(modifyFilter()));

		auto addFilterDoesNotContainAction = new QAction(boxWidget);
		addFilterDoesNotContainAction->setData(QVariant(i + addFilterDoesNotContainMask));
		auto addFilterDoesNotContainButton = new QPushButton(addFilterDoesNotContainIcon, "", boxWidget);
		addFilterDoesNotContainButton->setMaximumSize(buttonSize, buttonSize);
		addFilterDoesNotContainButton->setFlat(true);
		addFilterDoesNotContainButton->setToolTip(tr("Hide potions containing this item"));
		connect(addFilterDoesNotContainButton, SIGNAL(clicked(bool)), addFilterDoesNotContainAction, SLOT(trigger()));
		connect(addFilterDoesNotContainAction, SIGNAL(triggered(bool)), this, SLOT(modifyFilter()));

		auto removeFilterAction = new QAction(boxWidget);
		removeFilterAction->setData(QVariant(i + removeFilterMask));
		auto removeFilterButton = new QPushButton(removeFilterIcon, "", boxWidget);
		removeFilterButton->setMaximumSize(buttonSize, buttonSize);
		removeFilterButton->setFlat(true);
		connect(removeFilterButton, SIGNAL(clicked(bool)), removeFilterAction, SLOT(trigger()));
		connect(removeFilterAction, SIGNAL(triggered(bool)), this, SLOT(modifyFilter()));
		removeFilterButton->hide();

		boxLayout->addWidget(addFilterContainsButton);
		boxLayout->addWidget(removeFilterButton);
		boxLayout->addWidget(addFilterDoesNotContainButton);

		auto label = new QLabel(m_items[i]);
		label->setToolTip(m_tooltips[i]);
		boxLayout->addWidget(label);

		boxWidget->setLayout(boxLayout);
		mainLayout->addWidget(boxWidget);

		ItemBox itemBox;
		itemBox.widget = boxWidget;
		itemBox.addContainsButton = addFilterContainsButton;
		itemBox.addDoesNotContainButton = addFilterDoesNotContainButton;
		itemBox.removeButton = removeFilterButton;
		m_itemBoxes.push_back(itemBox);
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
			auto& itemBox = m_itemBoxes[id];

			if (type == FilterActionType::addFilterContains)
			{
				itemBox.addContainsButton->hide();
				itemBox.addDoesNotContainButton->show();
				itemBox.removeButton->show();
				itemBox.widget->setBackgroundRole(QPalette::Midlight);
				itemBox.widget->setAutoFillBackground(true);
			}
			else if (type == FilterActionType::addFilterDoesNotContain)
			{
				itemBox.addContainsButton->show();
				itemBox.addDoesNotContainButton->hide();
				itemBox.removeButton->show();
				itemBox.widget->setBackgroundRole(QPalette::Midlight);
				itemBox.widget->setAutoFillBackground(true);
			}
			else if (type == FilterActionType::RemoveFilter)
			{
				itemBox.addContainsButton->show();
				itemBox.addDoesNotContainButton->show();
				itemBox.removeButton->hide();
				itemBox.widget->setBackgroundRole(QPalette::Window);
				itemBox.widget->setAutoFillBackground(false);
			}

			filterAction(type, id);
		}
	}
}

void SelectorWidget::filterRemoved(int id)
{
	auto& itemBox = m_itemBoxes[id];
	itemBox.addContainsButton->show();
	itemBox.addDoesNotContainButton->show();
	itemBox.removeButton->hide();
	itemBox.widget->setBackgroundRole(QPalette::Window);
	itemBox.widget->setAutoFillBackground(false);
}

//****************************************************************************//

EffectsSelector::EffectsSelector(QWidget* parent)
	: SelectorWidget(parent)
{
	updateList();
}

void EffectsSelector::updateList()
{
	const auto& config = Config::main();

	QStringList names, tooltips;
	for (const auto& effect : config.effects)
	{
		names << effect.name;
		tooltips << effect.tooltip;
	}
	setItems(names, tooltips);
}

bool EffectsSelector::filterAction(FilterActionType action, int id)
{
	emit effectFilterAction(action, id);
	return true;
}

//****************************************************************************//

IngredientsSelector::IngredientsSelector(QWidget* parent)
	: SelectorWidget(parent)
{
	updateList();
}

void IngredientsSelector::updateList()
{
	const auto& config = Config::main();

	QStringList names, tooltips;
	for (const auto& ingredient : config.ingredients)
	{
		names << ingredient.name;
		tooltips << ingredient.tooltip;
	}
	setItems(names, tooltips);
}

bool IngredientsSelector::filterAction(FilterActionType action, int id)
{
	emit ingredientFilterAction(action, id);
	return false;
}
