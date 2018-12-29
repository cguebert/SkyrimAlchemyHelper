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
#include "DiscoverEffects.h"
#include "Config.h"
#include "GameSave.h"
#include "PotionsList.h"

std::pair<PotionsList::PotionsId, PotionsList::PotionAdditionalDataList> DiscoverEffects::selectPotions(int nb)
{
	DiscoverEffects de(nb);
	de.doComputation();
	return std::make_pair(de.m_selectedPotions, de.m_additionalData);
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
	m_additionalData.clear();

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
		const auto& ingredients = Config::main().ingredients;
		for (auto ingId : potion.ingredients)
		{
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
		const auto& ingredients = Config::main().ingredients;
		float score = 0;
		for (auto ingId : potion.ingredients)
		{
			if (ingId == -1)
				break;
			int nbUnknownEffects = 0, nbDiscoveredEffects = 0; // For this ingredient only
			const auto& ing = ingredients[ingId];
			for (int i = 0; i < Config::nbEffectsPerIngredient; ++i)
			{
				const auto& effData = ing.effects[i];
				if (m_knownIngredients[ingId][i])
					continue;

				++nbUnknownEffects;

				for (auto effId : potion.effects)
				{
					if (effId == -1)
						break;
					if (effData.effectId == effId)
						++nbDiscoveredEffects;
				}
			}
			
			if (nbDiscoveredEffects)
			{
				score += nbDiscoveredEffects * nbDiscoveredEffects; // Give more importance for many unknown effects in a single ingredient

				// If there is only one ingredient of this type, prefer potions that reveal all remaining unknown effects
				if (m_ingredientsCount[ingId] == 1 && nbUnknownEffects == nbDiscoveredEffects)
					score += 10;
			}

			// Prefer ingredients that have an higher count
			score += std::min<size_t>(20, m_ingredientsCount[ingId]) / 20.0;
		}

		// Prefer 2 ingredients potions if they have the same score
		if (potion.ingredients[2] == -1)
			score += 1.5;
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

	size_t selection = sorted.front();
	m_selectedPotions.push_back(selection);

	// Modify the lists as if the player has created this potion
	const auto& potions = potionsList.allPotions();
	const auto& potion = potions[selection];
	const auto& ingredients = Config::main().ingredients;

	PotionsList::PotionAdditionalData addData;

	for (int i = 0; i < PotionsList::maxIngredientsPerPotion; ++i)
	{
		const auto ingId = potion.ingredients[i];
		if (ingId == -1)
			break;

		addData.ingredientsCount[i] = m_ingredientsCount[ingId];
		--m_ingredientsCount[ingId]; // Use one of this ingredient
		
		const auto& ing = ingredients[ingId];
		for (int j = 0; j < Config::nbEffectsPerIngredient; ++j)
		{
			const auto& effData = ing.effects[j];
			for (auto effId : potion.effects)
			{
				if (effId == -1)
					break;

				if (effData.effectId == effId && !m_knownIngredients[ingId][j])
				{
					m_knownIngredients[ingId][j] = true; // Discover all effects of this potion
					addData.discoveredEffects.emplace_back(ingId, effId);
				}
			}
		}
	}

	m_additionalData.push_back(addData);

	return true;
}

void DiscoverEffects::cleanup()
{
	auto& potionsList = PotionsList::instance();
	potionsList.clearCustomFilters();
	potionsList.clearCustomSorts();
}
