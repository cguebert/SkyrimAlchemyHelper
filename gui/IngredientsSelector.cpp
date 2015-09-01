#include "IngredientsSelector.h"
#include "IngredientsList.h"

IngredientsSelector::IngredientsSelector(QWidget* parent)
	: SelectorWidget(parent)
{
	updateList();
}

void IngredientsSelector::updateList()
{
	const auto& ingredients = IngredientsList::instance().ingredients();

	QStringList list;
	for (const auto& ingredient : ingredients)
		list << ingredient.name;
	setItems(list);
}

bool IngredientsSelector::filterAction(FilterActionType action, int id)
{
	emit ingredientFilterAction(action, id);
	return false;
}
