/******************************************************************************
*                            Skyrim Alchemy Helper                            *
*******************************************************************************
*                                                                             *
* Copyright (C) 2015 Christophe Guebert                                       *
*                                                                             *
* This program is free software; you can redistribute it and/or modify        *
* it under the terms of the GNU General Public License as published by        *
* the Free Software Foundation; either version 2 of the License, or           *
* (at your option) any later version.                                         *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc.,     *
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                 *
*******************************************************************************
*                        Skyrim Alchemy Helper :: Gui                         *
*                                                                             *
*                  Contact: christophe.guebert+SAH@gmail.com                  *
******************************************************************************/
#include "PotionsList.h"
#include "Config.h"
#include "GameSave.h"
#include "DiscoverEffects.h"

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

	const auto& ingredients = Config::main().ingredients;
	const auto& effects = Config::main().effects;

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
			for (int i = 0; i < Config::nbEffectsPerIngredient; ++i)
			{
				auto effect1 = ing1.sortedEffects[i].effectId;
				for (int j = 0; j < Config::nbEffectsPerIngredient; ++j)
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
	updateEffectsToxicity();
	saveList();

	m_currentFilters.clear();
	applyFilters();

	prepareDefaultSortFunctions();
	sortPotions();
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
	prepareDefaultSortFunctions();
}

void PotionsList::applyFilters()
{
	int nb = m_allPotions.size();
	m_filteredPotions.clear();
	m_filteredPotions.reserve(nb);

	for (int i = 0; i < nb; ++i)
	{
		const auto& potion = m_allPotions[i];
		if (!defaultFilters(potion))
			continue;

		bool keep = true;
		for (auto func : m_customFilterFunctions)
		{
			if (!func(potion))
			{
				keep = false;
				break;
			}
		}

		if (keep)
			m_filteredPotions.push_back(i);
	}
}

void PotionsList::sortPotions()
{
	m_sortedPotions = m_filteredPotions;

	std::vector<float> scores(m_allPotions.size(), 0.f);
	const float factor = 2.0; // Give more importance to the firt criterion

	if (m_customSortFunctions.empty())
	{
		for (auto func : m_defaultSortFunctions)
			for (const auto id : m_sortedPotions)
				scores[id] = scores[id] * factor + func(m_allPotions[id]);
	}
	else
	{
		for (auto func : m_customSortFunctions)
			for (const auto id : m_sortedPotions)
				scores[id] = scores[id] * factor + func(m_allPotions[id]);
	}

	std::sort(m_sortedPotions.begin(), m_sortedPotions.end(), [&scores](int lhs, int rhs){
		return scores[lhs] > scores[rhs];
	});

	computePotionsData();
}

bool PotionsList::defaultFilters(const Potion& potion)
{
	using FilterType = Filter::FilterType;
	for (const auto& filter : m_currentFilters)
	{
		switch (filter.type)
		{

		case FilterType::HasIngredient:
		{
			bool contains = false;
			for (int i = 0; i < maxIngredientsPerPotion; ++i)
				if (potion.ingredients[i] == filter.data)
				{
					contains = true;
					break;
				}
			if (!contains)
				return false;
			break;
		}

		case FilterType::DoesNotHaveIngredient:
		{
			for (int i = 0; i < maxIngredientsPerPotion; ++i)
				if (potion.ingredients[i] == filter.data)
					return false;
			break;
		}

		case FilterType::HasEffect:
		{
			bool contains = false;
			for (int i = 0; i < maxEffectsPerPotion; ++i)
				if (potion.effects[i] == filter.data)
				{
					contains = true;
					break;
				}
			if (!contains)
				return false;
			break;
		}

		case FilterType::DoesNotHaveEffect:
		{
			for (int i = 0; i < maxEffectsPerPotion; ++i)
				if (potion.effects[i] == filter.data)
					return false;
			break;
		}

		case FilterType::TwoIngredients:
		{
			if (potion.ingredients[2] != -1)
				return false;
			break;
		}

		case FilterType::ThreeIngredients:
		{
			if (potion.ingredients[2] == -1)
				return false;
			break;
		}

		case FilterType::AvailableIngredients:
		{
			const auto& save = GameSave::instance();
			if (!save.isLoaded())
				break;

			const auto& ingredientsCount = save.ingredientsCount();
			if (ingredientsCount.empty())
				break;

			for (int i = 0; i < maxIngredientsPerPotion; ++i)
				if (potion.ingredients[i] != -1 && !ingredientsCount[potion.ingredients[i]])
					return false;
			break;
		}
		case FilterType::Pure:
		{
			bool toxicity = m_toxicity[potion.effects[0]];
			for (int i = 1; i < maxEffectsPerPotion; ++i)
				if (potion.effects[i] != -1 && m_toxicity[potion.effects[i]] != toxicity)
					return false;
			break;
		}

		case FilterType::PurePositive:
		{
			for (int i = 0; i < maxEffectsPerPotion; ++i)
				if (potion.effects[i] != -1 && m_toxicity[potion.effects[i]])
					return false;
			break;
		}

		case FilterType::PureNegative:
		{
			for (int i = 0; i < maxEffectsPerPotion; ++i)
				if (potion.effects[i] != -1 && !m_toxicity[potion.effects[i]])
					return false;
			break;
		}

		default:
			return false;

		} // switch
	}
	return true;
}

void PotionsList::computePotionsStrength()
{
	const auto& ingredients = Config::main().ingredients;
	const auto& effects = Config::main().effects;
	m_maxGoldPotion = 0;
	maxEffectMagnitude.assign(effects.size(), 0);
	maxEffectDuration.assign(effects.size(), 0);
	const float powerFactor = 6; // For alchemy @ 100, no perks

	for (auto& potion : m_allPotions)
	{
		potion.goldCost = 0;
		for (int i = 0; i < maxEffectsPerPotion; ++i)
		{
			auto effId = potion.effects[i];
			if (effId == -1)
				break;

			// We look for the ingredient with the highest gold cost for this effect
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
						float strength = (ingEff.magnitude > 1 ? ingEff.magnitude : 1) * (ingEff.duration > 0 ? ingEff.duration : 1);
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

			// Then compute the real values for this effect
			float &magnitude = potion.magnitudes[i], &duration = potion.durations[i];
			if (effect.flags & EffectFlags::PowerAffectsMagnitude)
				magnitude *= powerFactor;
			if (effect.flags & EffectFlags::PowerAffectsDuration)
				duration *= powerFactor;

			magnitude = std::round(magnitude);
			duration = std::round(duration);

			float magCost = 1, durCost = 1;
			if (magnitude > 1)
				magCost = pow(magnitude, 1.1f);
			if (duration > 0)
				durCost = 0.0794328 * pow(duration, 1.1f);
			potion.goldCost += effect.baseCost * magCost * durCost;

			if (magnitude > maxEffectMagnitude[effId])
				maxEffectMagnitude[effId] = magnitude;

			if (duration > maxEffectDuration[effId])
				maxEffectDuration[effId] = duration;
		}

		potion.goldCost = std::floor(potion.goldCost);
		if (potion.goldCost > m_maxGoldPotion)
			m_maxGoldPotion = potion.goldCost;
	}
}

void PotionsList::updateEffectsToxicity()
{
	const auto& effects = Config::main().effects;
	int nb = effects.size();
	m_toxicity.resize(nb);

	for (int i = 0; i < nb; ++i)
	{
		const auto& effect = effects[i];
		m_toxicity[i] = ((effect.flags & EffectFlags::Hostile) != 0);
	}
}

void PotionsList::prepareDefaultSortFunctions()
{
	m_defaultSortFunctions.clear();
	const auto& effects = Config::main().effects;

	bool hasEffectFilter = false;
	for (const auto& filter : m_currentFilters)
	{
		if (filter.type == Filter::FilterType::HasEffect)
		{
			int effectId = filter.data;
			
			// Sorting by magnitude
			if (effects[effectId].flags & EffectFlags::PowerAffectsMagnitude)
			{
				float maxMag = maxEffectMagnitude[effectId];
				if (maxMag > 0)
					m_defaultSortFunctions.push_back([maxMag, effectId](const Potion& potion){
						for (int i = 0; i < maxEffectsPerPotion; ++i)
							if (potion.effects[i] == effectId)
								return potion.magnitudes[i] / maxMag;
						return 0.f;
					});
			}

			// Sorting by duration
			if (effects[effectId].flags & EffectFlags::PowerAffectsDuration)
			{
				float maxDur = maxEffectDuration[effectId];
				if(maxDur > 0)
					m_defaultSortFunctions.push_back([maxDur, effectId](const Potion& potion){
						for (int i = 0; i < maxEffectsPerPotion; ++i)
							if (potion.effects[i] == effectId)
								return potion.durations[i] / maxDur;
						return 0.f;
					});
			}
		}
	}

	// Last, sort by gold cost
	auto maxGold = m_maxGoldPotion;
	m_defaultSortFunctions.push_back([maxGold](const Potion& potion){
		return potion.goldCost / maxGold;
	});
}

void PotionsList::computePotionsData()
{
	m_additionalData.clear();

	if (!GameSave::instance().isLoaded())
		return;

	const auto& ingredients = Config::main().ingredients;
	const auto& ingredientsCount = GameSave::instance().ingredientsCount();
	const auto& knownIngredients = GameSave::instance().knownIngredients();

	int nb = std::min(static_cast<int>(m_sortedPotions.size()), m_nbComputePotionsData);
	m_additionalData.reserve(nb);

	for (int i = 0; i < nb; ++i)
	{
		const auto& potion = m_allPotions[m_sortedPotions[i]];
		PotionAdditionalData addData;

		for (int j = 0; j < maxIngredientsPerPotion; ++j)
		{
			int ingId = potion.ingredients[j];
			addData.ingredientsCount[j] = (ingId != -1 ? ingredientsCount[ingId] : 0);

			if (ingId == -1)
				break;
			const auto& ing = ingredients[ingId];
			for (int i = 0; i < Config::nbEffectsPerIngredient; ++i)
			{
				const auto& effData = ing.effects[i];
				for (auto effId : potion.effects)
				{
					if (effId == -1)
						break;

					if (effData.effectId == effId && !knownIngredients[ingId][i])
						addData.discoveredEffects.emplace_back(ingId, effId);
				}
			}
		}

		m_additionalData.push_back(addData);
	}
}

void PotionsList::discoverEffects()
{
	std::tie(m_sortedPotions, m_additionalData) = DiscoverEffects::selectPotions();
}
