#ifndef POTIONSLIST_H
#define POTIONSLIST_H

#include <functional>
#include <vector>

class PotionsList
{
public:
	static PotionsList& instance();

	void recomputeList();

	static const int maxIngredientsPerPotion = 3;
	static const int maxEffectsPerPotion = 6;

	struct Potion
	{
		Potion()
		{
			for (int i = 0; i < maxIngredientsPerPotion; ++i) ingredients[i] = -1;
			for (int i = 0; i < maxEffectsPerPotion; ++i) { effects[i] = -1; magnitudes[i] = durations[i] = 0; }
		}

		int potionId = -1;
		int ingredients[maxIngredientsPerPotion];
		int effects[maxEffectsPerPotion];
		float magnitudes[maxEffectsPerPotion];
		float durations[maxEffectsPerPotion];
		float goldCost = 0;
	};
	using Potions = std::vector<Potion>;
	using PotionsId = std::vector<int>;

	const Potions& allPotions() const;
	const PotionsId& sortedPotions() const;

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

	using FilterFunc = std::function<bool(const Potion&)>; // Do we keep this potion or not
	using SortFunc = std::function<float(const Potion&)>; // Score of this potion [0:1] (higher is first)
	using SortFunctions = std::vector<SortFunc>;

protected:
	PotionsList();
	bool loadList();
	void saveList();

	void applyFilters();
	void sortPotions();
	void computePotionsStrength();
	void updateEffectsToxicity();

	bool defaultFilters(const Potion& potion);
	void prepareDefaultSortFunctions();
	
	Potions m_allPotions;
	PotionsId m_filteredPotions, m_sortedPotions;
	Filters m_currentFilters;
	std::vector<bool> m_toxicity;
	SortFunctions m_defaultSortFunctions;
	float m_maxGoldPotion = 0;
	std::vector<float> maxEffectMagnitude, maxEffectDuration;
};

//****************************************************************************//

inline const PotionsList::Potions& PotionsList::allPotions() const
{ return m_allPotions; }

inline const PotionsList::PotionsId& PotionsList::sortedPotions() const
{ return m_sortedPotions; }

inline const PotionsList::Filters& PotionsList::filters() const
{ return m_currentFilters; }

#endif // POTIONSLIST_H
