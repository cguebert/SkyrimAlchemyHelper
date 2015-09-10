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

		QString tooltip;
		for (const auto& effect : ingredient.sortedEffects)
			tooltip += effects[effect.effectId].name + "\n";
		tooltips << tooltip.trimmed();
	}
	setItems(names, tooltips);
}

bool IngredientsSelector::filterAction(FilterActionType action, int id)
{
	emit ingredientFilterAction(action, id);
	return false;
}
