#include <QtWidgets>

#include "FiltersWidget.h"
#include "FlowLayout.h"

#include "EffectsList.h"
#include "IngredientsList.h"

FiltersWidget::FiltersWidget(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(0, 0, 0, 0);

	m_flowLayout = new FlowLayout(nullptr, 0, 0, 0);
	m_flowLayout->setSpacing(0);
	vLayout->addLayout(m_flowLayout);

	setLayout(vLayout);
}

void FiltersWidget::clear()
{
	while (auto child = m_flowLayout->takeAt(0))
		delete child;
}

bool FiltersWidget::updateExisting(std::vector<ItemTuple>& list, FilterActionType action, int id)
{
	auto it = std::find_if(list.begin(), list.end(), [id](const ItemTuple& t){
		return std::get<1>(t) == id;
	});
	if (it != list.end())
	{
		auto oldAction = std::get<0>(*it);
		auto widget = std::get<2>(*it);
		switch (action)
		{
		case FilterActionType::addFilterContains:
			if (oldAction == FilterActionType::addFilterContains)
				return false;
			else if (oldAction == FilterActionType::addFilterDoesNotContain)
			{
				list.erase(it);
				removeWidget(widget);
			}
			break;
		case FilterActionType::addFilterDoesNotContain:
			if (oldAction == FilterActionType::addFilterDoesNotContain)
				return false;
			else if (oldAction == FilterActionType::addFilterContains)
			{
				list.erase(it);
				removeWidget(widget);
			}
			break;
		case FilterActionType::RemoveFilter:
			list.erase(it);
			removeWidget(widget);
			return false;
		}
	}

	return true;
}

void FiltersWidget::effectFilterAction(FilterActionType action, int id)
{
	// Do we already have this effect ?
	if (!updateExisting(m_effects, action, id))
		return;

	if (action == FilterActionType::RemoveFilter)
		return;

	// Add this effect
	const auto& effect = EffectsList::instance().effect(id);

	auto boxWidget = new QWidget;
	auto boxLayout = new QHBoxLayout(boxWidget);
	boxLayout->setContentsMargins(10, 5, 5, 5);

	auto removeAction = new QAction(boxLayout);
	removeAction->setData(QVariant(id));
	auto removeButton = new QPushButton(QIcon("://icons/Delete.png"), "");
	removeButton->setMaximumSize(24, 24);
	removeButton->setFlat(true);
	connect(removeButton, SIGNAL(clicked(bool)), removeAction, SLOT(trigger()));
	connect(removeAction, SIGNAL(triggered(bool)), this, SLOT(removeEffect()));
	boxLayout->addWidget(removeButton);

	auto label = new QLabel((action == FilterActionType::addFilterContains ? "Has " : "Does not have ") + effect.name);
	boxLayout->addWidget(label);

	m_flowLayout->addWidget(boxWidget);

	m_effects.emplace_back(action, id, boxWidget);
}

void FiltersWidget::ingredientFilterAction(FilterActionType action, int id)
{
	// Do we already have this ingredient ?
	if (!updateExisting(m_ingredients, action, id))
		return;

	if (action == FilterActionType::RemoveFilter)
		return;

	// Add this ingredient
	const auto& ingredient = IngredientsList::instance().ingredient(id);

	auto boxWidget = new QWidget;
	auto boxLayout = new QHBoxLayout(boxWidget);
	boxLayout->setContentsMargins(10, 5, 5, 5);

	auto removeAction = new QAction(boxLayout);
	removeAction->setData(QVariant(id));
	auto removeButton = new QPushButton(QIcon("://icons/Delete.png"), "");
	removeButton->setMaximumSize(24, 24);
	removeButton->setFlat(true);
	connect(removeButton, SIGNAL(clicked(bool)), removeAction, SLOT(trigger()));
	connect(removeAction, SIGNAL(triggered(bool)), this, SLOT(removeIngredient()));
	boxLayout->addWidget(removeButton);

	auto label = new QLabel((action == FilterActionType::addFilterContains ? "Has " : "Does not have ") + ingredient.name);
	boxLayout->addWidget(label);

	m_flowLayout->addWidget(boxWidget);

	m_ingredients.emplace_back(action, id, boxWidget);
}

void FiltersWidget::removeWidget(QWidget* widget)
{
	int nb = m_flowLayout->count();
	for (int i = 0; i < nb; ++i)
	{
		auto item = m_flowLayout->itemAt(i);
		if (item && item->widget() == widget)
		{
			auto item = m_flowLayout->takeAt(i);
			auto w = item->widget();
			if (w) delete w;
			delete item;
			m_flowLayout->update();
			return;
		}
	}
}

void FiltersWidget::removeEffect()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
	{
		bool ok = false;
		int id = action->data().toInt(&ok);
		if (ok)
		{
			auto it = std::find_if(m_effects.begin(), m_effects.end(), [id](const ItemTuple& t){
				return std::get<1>(t) == id;
			});
			if (it != m_effects.end())
				removeWidget(std::get<2>(*it));
		}
	}
}

void FiltersWidget::removeIngredient()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
	{
		bool ok = false;
		int id = action->data().toInt(&ok);
		if (ok)
		{
			auto it = std::find_if(m_ingredients.begin(), m_ingredients.end(), [id](const ItemTuple& t){
				return std::get<1>(t) == id;
			});
			if (it != m_ingredients.end())
				removeWidget(std::get<2>(*it));
		}
	}
}