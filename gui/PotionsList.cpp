#include "PotionsList.h"
#include "IngredientsList.h"
#include "EffectsList.h"

#include <algorithm>
#include <iterator>
#include <limits>

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

int matTo1d(int x, int y, int N)
{
	if (x <= y)
		return x * N - (x - 1) * x / 2 + y - x;
	else
		return y * N - (y - 1) * y / 2 + x - y;
}

void PotionsList::recomputeList()
{
	m_allPotions.clear();

	const auto& ingredients = IngredientsList::instance().ingredients();
	const auto& effects = EffectsList::instance().effects();

	const int nbIng = ingredients.size();
	const int nbEff = effects.size();
	const int maxInt = INT_MAX; // std::numeric_limits<int>::max()

	// Naive implementation
	// First create potions with only 2 ingredients
	std::vector<int> potionsId_2Ing(nbIng * (nbIng + 1) / 2, -1); // Store the results for later
	int potId = 0;
	
	for (int id1 = 0; id1 < nbIng; ++id1)
	{
		const auto& ing1 = ingredients[id1];
		for (int id2 = id1 + 1; id2 < nbIng; ++id2)
		{
			const auto& ing2 = ingredients[id2];
			Potion potion;
			potion.ingredients[0] = id1;
			potion.ingredients[1] = id2;

			int nbEffects = 0;
			for (int i = 0; i < IngredientsList::nbEffectsPerIngredient; ++i)
			{
				auto effect1 = ing1.sortedEffects[i].effectId;
				for (int j = 0; j < IngredientsList::nbEffectsPerIngredient; ++j)
				{
					if (effect1 == ing2.sortedEffects[j].effectId)
						potion.effects[nbEffects++] = effect1;
				}
			}

			if (nbEffects)
			{
				potionsId_2Ing[matTo1d(id1, id2, nbIng)] = potId++;
				m_allPotions.push_back(potion);
			}
		}
	}

	int emptyEffects[maxEffectsPerPotion];
	for (int i = 0; i < maxEffectsPerPotion; ++i) emptyEffects[i] = -1;

	// Then create combinaisons of these potions
	for (int id1 = 0; id1 < nbIng; ++id1)
	{
		for (int id2 = id1 + 1; id2 < nbIng; ++id2)
		{
			int pot12Id = potionsId_2Ing[matTo1d(id1, id2, nbIng)];
			if (pot12Id == -1)
				continue;

			for (int id3 = id2 + 1; id3 < nbIng; ++id3)
			{
				int pot13Id = potionsId_2Ing[matTo1d(id1, id3, nbIng)];
				int pot23Id = potionsId_2Ing[matTo1d(id2, id3, nbIng)];

				if (pot13Id == -1 && pot23Id == -1)
					continue;

				const auto& eff1 = m_allPotions[pot12Id].effects;
				const auto& eff2 = pot13Id != -1 ? m_allPotions[pot13Id].effects : emptyEffects;
				const auto& eff3 = pot23Id != -1 ? m_allPotions[pot23Id].effects : emptyEffects;

				Potion potion;
				potion.ingredients[0] = id1;
				potion.ingredients[1] = id2;
				potion.ingredients[2] = id3;

				int nbEffects = 0, p1 = 0, p2 = 0, p3 = 0;
				while (true)
				{
					int e1 = eff1[p1], e2 = eff2[p2], e3 = eff3[p3];
					if (e1 == -1 && e2 == -1 && e3 == -1)
						break;

					if (e1 == -1) e1 = maxInt; 
					if (e2 == -1) e2 = maxInt;
					if (e3 == -1) e3 = maxInt;
					
					int eMin = std::min({ e1, e2, e3 });
					int nb = 0;
					if (eMin == e1) { ++nb; ++p1; }
					if (eMin == e2) { ++nb; ++p2; }
					if (eMin == e3) { ++nb; ++p3; }

					if (nb)
						potion.effects[nbEffects++] = eMin;
				}

				m_allPotions.push_back(potion);
			}
		}
	}
	
	computePotionsStrength();
	saveList();

	m_currentFilters.clear();
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

void PotionsList::computePotionsStrength()
{
	const auto& ingredients = IngredientsList::instance().ingredients();
	const auto& effects = EffectsList::instance().effects();

	for (auto& potion : m_allPotions)
	{
		for (int i = 0; i < maxEffectsPerPotion; ++i)
		{
			auto effId = potion.effects[i];
			if (effId == -1)
				break;

			const auto& effect = effects[effId];
			float maxStrength = 0;
			for (auto ingId : potion.ingredients)
			{
				if (ingId == -1)
					break;

				const auto& ing = ingredients[ingId];
				for (const auto& ingEff : ing.effects)
				{
					if (ingEff.effectId == effId)
					{
						float strength = (ingEff.magnitude > 1 ? ingEff.magnitude : 1) * (ingEff.duration > 0 ? ingEff.duration / 10 : 1);
						if (strength > maxStrength)
						{
							potion.magnitudes[i] = ingEff.magnitude;
							potion.durations[i] = ingEff.duration;
							maxStrength = strength;
						}
						break;
					}
				}
			}
		}
	}
}