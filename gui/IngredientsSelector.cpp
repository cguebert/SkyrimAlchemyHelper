#include "IngredientsSelector.h"
#include "IngredientsList.h"
#include "EffectsList.h"

IngredientsSelector::IngredientsSelector(QWidget* parent)
	: SelectorWidget(parent)
{
	updateList();
}

void IngredientsSelector::updateList()
{
	const auto& ingredients = IngredientsList::instance().ingredients();
	const auto& effects = EffectsList::instance().effects();

	QStringList names, tooltips;
	for (const auto& ingredient : ingredients)
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
