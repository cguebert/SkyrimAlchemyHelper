#include "DiscoverEffects.h"
#include "IngredientsList.h"
#include "GameSave.h"
#include "PotionsList.h"

std::vector<int> DiscoverEffects::selectPotions(int nb)
{
	DiscoverEffects de(nb);
	de.doComputation();
	return de.m_selectedPotions;
}

DiscoverEffects::DiscoverEffects(int nbPotions)
	: m_nbPotions(nbPotions)
{
}

void DiscoverEffects::doComputation()
{
	prepare();
	
	for (int i = 0; i < m_nbPotions; ++i)
		if (!selectOnePotion())
			break;

	cleanup();
}

void DiscoverEffects::prepare()
{
	const auto& save = GameSave::instance();
	m_ingredientsCount = save.ingredientsCount();
	m_knownIngredients = save.knownIngredients();
	m_selectedPotions.clear();

	auto& potionsList = PotionsList::instance();
	PotionsList::Filters emptyFilters;
	potionsList.setFilters(emptyFilters);
	potionsList.clearCustomFilters();
	potionsList.clearCustomSorts();

	setFilters();
	setSortingFunction();
}

void DiscoverEffects::setFilters()
{
	auto& potionsList = PotionsList::instance();

	// Filter for available ingredients
	potionsList.addCustomFilter([this](const PotionsList::Potion& potion){
		for (int i = 0; i < PotionsList::maxIngredientsPerPotion; ++i)
			if (potion.ingredients[i] != -1 && !m_ingredientsCount[potion.ingredients[i]])
				return false;
		return true;
	});

	// Then filter for at least one unknwon ingredient
	potionsList.addCustomFilter([this](const PotionsList::Potion& potion){
		const auto& ingredients = IngredientsList::instance().ingredients();
		for (auto ingId : potion.ingredients)
		{
			if (ingId == -1)
				break;
			const auto& ing = ingredients[ingId];
			for (int i = 0; i < IngredientsList::nbEffectsPerIngredient; ++i)
			{
				const auto& effData = ing.effects[i];
				for (auto effId : potion.effects)
				{
					if (effId == -1)
						break;

					if (effData.effectId == effId && !m_knownIngredients[ingId][i])
						return true;
				}
			}
		}
		return false;
	});
}

void DiscoverEffects::setSortingFunction()
{
	auto& potionsList = PotionsList::instance();
	potionsList.addCustomSort([this](const PotionsList::Potion& potion){
		const auto& ingredients = IngredientsList::instance().ingredients();
		float score = 0;
		for (auto ingId : potion.ingredients)
		{
			if (ingId == -1)
				break;
			int nbUnknownEffects = 0, nbDiscoveredEffects = 0; // For this ingredient only
			const auto& ing = ingredients[ingId];
			for (int i = 0; i < IngredientsList::nbEffectsPerIngredient; ++i)
			{
				const auto& effData = ing.effects[i];
				for (auto effId : potion.effects)
				{
					if (effId == -1)
						break;
					if (!m_knownIngredients[ingId][i])
						++nbUnknownEffects;
					if (effData.effectId == effId && !m_knownIngredients[ingId][i])
						++nbDiscoveredEffects;
				}
			}
			score += nbDiscoveredEffects * nbDiscoveredEffects; // Give more importance for many unknown effects in a single ingredient
			// If there is only one ingredient of this type, prefer potions that reveal all remaining unknown effects
			if (m_ingredientsCount[ingId] == 1 && nbUnknownEffects == nbDiscoveredEffects)
				score += 10;
		}

		// Prefer 2 ingredients potions if they have the same score
		if (potion.ingredients[2] == -1)
			score += 3.5;
		return score;
	});
}

bool DiscoverEffects::selectOnePotion()
{
	auto& potionsList = PotionsList::instance();
	potionsList.applyFilters();
	potionsList.sortPotions();

	const auto& sorted = potionsList.sortedPotions();
	if (sorted.empty())
		return false;

	int selection = sorted.front();
	m_selectedPotions.push_back(selection);

	// Modify the lists as if the player has created this potion
	const auto& potions = potionsList.allPotions();
	const auto& potion = potions[selection];
	const auto& ingredients = IngredientsList::instance().ingredients();

	for (auto ingId : potion.ingredients)
	{
		if (ingId == -1)
			break;

		--m_ingredientsCount[ingId]; // Use one of this ingredient
		const auto& ing = ingredients[ingId];
		for (int i = 0; i < IngredientsList::nbEffectsPerIngredient; ++i)
		{
			const auto& effData = ing.effects[i];
			for (auto effId : potion.effects)
			{
				if (effId == -1)
					break;

				if (effData.effectId == effId)
					m_knownIngredients[ingId][i] = true; // Discover all effects of this potion
			}
		}
	}

	return true;
}

void DiscoverEffects::cleanup()
{
	auto& potionsList = PotionsList::instance();
	potionsList.clearCustomFilters();
	potionsList.clearCustomSorts();
}
