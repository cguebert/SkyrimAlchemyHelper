#include "Ingredients.h"
#include <algorithm>

bool Ingredients::setIngredient(const Ingredient& ingredient)
{
	auto it = std::find_if(m_ingredients.begin(), m_ingredients.end(), [&ingredient](const Ingredient& ing){
		return ingredient.id == ing.id && ingredient.modName == ing.modName;
	});

	if (it != m_ingredients.end())
	{
		*it = ingredient;
		return false; // false if modifying
	}
	else
	{
		m_ingredients.push_back(ingredient);
		return true; // true if adding
	}
}

const std::vector<Ingredients::Ingredient>& Ingredients::ingredients() const
{
	return m_ingredients;
}