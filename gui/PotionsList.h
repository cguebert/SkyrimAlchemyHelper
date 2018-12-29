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
#ifndef POTIONSLIST_H
#define POTIONSLIST_H

#include <functional>
#include <vector>

namespace EffectFlags
{
enum
{
	Hostile = 0x01,
	PowerAffectsMagnitude = 0x00200000,
	PowerAffectsDuration = 0x00400000
};
}

class PotionsList
{
public:
	static PotionsList& instance();

	void recomputeList();
	void applyFilters();
	void sortPotions();
	void discoverEffects(); // Make a selection of the best potions to discover unknwown effects

	static const int maxIngredientsPerPotion = 3;
	static const int maxEffectsPerPotion = 6;

	struct Potion
	{
		Potion()
		{
			for (int i = 0; i < maxIngredientsPerPotion; ++i) ingredients[i] = -1;
			for (int i = 0; i < maxEffectsPerPotion; ++i) { effects[i] = -1; magnitudes[i] = durations[i] = 0; }
		}

		size_t ingredients[maxIngredientsPerPotion];
		size_t effects[maxEffectsPerPotion];
		float magnitudes[maxEffectsPerPotion];
		float durations[maxEffectsPerPotion];
		float goldCost = 0;
	};
	using Potions = std::vector<Potion>;
	using PotionsId = std::vector<size_t>;

	const Potions& allPotions() const;
	const PotionsId& sortedPotions() const;

	struct PotionAdditionalData
	{
		size_t ingredientsCount[maxIngredientsPerPotion];
		using DiscoveredEffect = std::pair<size_t, size_t>; // Ingredient id, effect id
		std::vector<DiscoveredEffect> discoveredEffects;
	};
	using PotionAdditionalDataList = std::vector<PotionAdditionalData>;
	const PotionAdditionalDataList& additionalData() const; // Only for sorted potions
	void setNbComputePotionsData(int nb); // Set the number of additional data that must be computed (at least for all potions shown in PotionsListWidget)

	struct Filter
	{
		enum class FilterType
		{	
			None, 
			HasEffect, DoesNotHaveEffect, 
			HasIngredient, DoesNotHaveIngredient, 
			TwoIngredients, ThreeIngredients,
			AvailableIngredients, 
			Pure, PurePositive, PureNegative 
		};

		Filter() {}
		Filter(FilterType type, int data = 0) : type(type), data(data) {}

		FilterType type = FilterType::None;
		int data = 0; // Index of the ingredient or the effect
	};
	using Filters = std::vector<Filter>;

	void setFilters(const Filters& filters);
	const Filters& filters() const;

	using FilterFunction = std::function<bool(const Potion&)>; // Do we keep this potion or not
	void addCustomFilter(FilterFunction func); // Apply this filter on top of the standard ones
	void clearCustomFilters();

	using SortFunction = std::function<float(const Potion&)>; // Score of this potion [0:1] (higher is first)
	void addCustomSort(SortFunction func); // Replace the default ones
	void clearCustomSorts();

protected:
	PotionsList();
	bool loadList();
	void saveList();

	void computePotionsStrength();
	void updateEffectsToxicity();
	void computePotionsData();

	bool defaultFilters(const Potion& potion);
	void prepareDefaultSortFunctions();

	using FilterFunctions = std::vector<FilterFunction>;
	using SortFunctions = std::vector<SortFunction>;
	
	Potions m_allPotions;
	PotionsId m_filteredPotions, m_sortedPotions;
	Filters m_currentFilters;
	
	FilterFunctions m_customFilterFunctions;
	SortFunctions m_defaultSortFunctions, m_customSortFunctions;
	
	std::vector<bool> m_toxicity;
	float m_maxGoldPotion = 0;
	std::vector<float> maxEffectMagnitude, maxEffectDuration;

	int m_nbComputePotionsData = 50;
	PotionAdditionalDataList m_additionalData;
};

//****************************************************************************//

inline const PotionsList::Potions& PotionsList::allPotions() const
{ return m_allPotions; }

inline const PotionsList::PotionsId& PotionsList::sortedPotions() const
{ return m_sortedPotions; }

inline const PotionsList::Filters& PotionsList::filters() const
{ return m_currentFilters; }

inline void PotionsList::addCustomFilter(FilterFunction func)
{ m_customFilterFunctions.push_back(func); }

inline void PotionsList::clearCustomFilters()
{ m_customFilterFunctions.clear(); }

inline void PotionsList::addCustomSort(SortFunction func)
{ m_customSortFunctions.push_back(func); }

inline void PotionsList::clearCustomSorts()
{ m_customSortFunctions.clear(); }

inline const PotionsList::PotionAdditionalDataList& PotionsList::additionalData() const
{ return m_additionalData; }

inline void PotionsList::setNbComputePotionsData(int nb)
{ m_nbComputePotionsData = nb; }

#endif // POTIONSLIST_H
