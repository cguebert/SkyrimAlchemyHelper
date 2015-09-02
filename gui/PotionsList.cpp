#include "PotionsList.h"
#include "IngredientsList.h"
#include "EffectsList.h"

#include <algorithm>
#include <iterator>

PotionsList& PotionsList::instance()
{
	static PotionsList potions;
	return potions;
}

PotionsList::PotionsList()
{
	if (!loadList())
		recomputeList();
}

void PotionsList::recomputeList()
{
	m_allPotions.clear();

	const auto& ingredients = IngredientsList::instance().ingredients();
	const auto& effects = EffectsList::instance().effects();

	// Naive implementation first
	// First create potions with only 2 ingredients
	const int nb = ingredients.size();
	for (int id1 = 0; id1 < nb; ++id1)
	{
		const auto& ing1 = ingredients[id1];
		for (int id2 = id1 + 1; id2 < nb; ++id2)
		{
			const auto& ing2 = ingredients[id2];
			Potion potion;
			potion.ingredients[0] = id1;
			potion.ingredients[1] = id2;

			int nbEffects = 0;
			for (int i = 0; i < IngredientsList::nbEffectsPerIngredient; ++i)
			{
				auto effect1 = ing1.effects[i].effectId;
				for (int j = 0; j < IngredientsList::nbEffectsPerIngredient; ++j)
				{
					if (effect1 == ing2.effects[j].effectId)
						potion.effects[nbEffects++] = effect1;
				}
			}

			if (nbEffects)
				m_allPotions.push_back(potion);
		}
	}

	saveList();

	applyFilters();
	updateViews();
}

bool PotionsList::loadList()
{
	return false;
}

void PotionsList::saveList()
{

}

void PotionsList::setFilters(const Filters& filters)
{
	m_currentFilters = filters;
	applyFilters();
}

void PotionsList::applyFilters()
{
	m_filteredPotions.clear();
	std::copy_if(m_allPotions.begin(), m_allPotions.end(), std::back_inserter(m_filteredPotions), [this](const Potion& potion) {
		for (const auto& filter : m_currentFilters)
		{
			if (filter.isIngredient)
			{
				if (filter.contains)
				{
					bool contains = false;
					for (int i = 0; i < maxIngredientsPerPotion; ++i)
						if (potion.ingredients[i] == filter.id)
						{
							contains = true;
							break;
						}
					if (!contains)
						return false;
				}
				else
				{
					for (int i = 0; i < maxIngredientsPerPotion; ++i)
						if (potion.ingredients[i] == filter.id)
							return false;
				}
			}
			else
			{
				if (filter.contains)
				{
					bool contains = false;
					for (int i = 0; i < maxEffectsPerPotion; ++i)
						if (potion.effects[i] == filter.id)
						{
							contains = true;
							break;
						}
					if (!contains)
						return false;
				}
				else
				{
					for (int i = 0; i < maxEffectsPerPotion; ++i)
						if (potion.effects[i] == filter.id)
							return false;
				}
			}
		}
		return true;
	});

	updateViews();
}

void PotionsList::updateViews()
{
	if (m_updateCallback)
		m_updateCallback();
}