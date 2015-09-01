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

	ItemList list;
	for (const auto& ingredient : ingredients)
		list.emplace_back(ingredient.ingId, ingredient.name);
	setItems(std::move(list));
}

bool IngredientsSelector::setFilterContains(int32_t id)
{
	return false;
}

bool IngredientsSelector::setFilterDoesNotContain(int32_t id)
{
	return false;
}

bool IngredientsSelector::removeFilter(int32_t id)
{
	return false;
}
