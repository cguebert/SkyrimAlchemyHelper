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
	};
	using Potions = std::vector<Potion>;

	const Potions& allPotions() const;
	const Potions& filteredPotions() const;

	struct Filter
	{
		enum class FilterType
		{	
			None, 
			HasEffect, DoesNotHaveEffect, 
			HasIngredient, DoesNotHaveIngredient, 
			TwoIngredients, ThreeIngredients,
			AvailableOnly, 
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

	using Callback = std::function<void()>;
	void setUpdateCallback(Callback func);

protected:
	PotionsList();
	bool loadList();
	void saveList();
	void updateViews();
	void applyFilters();
	void computePotionsStrength();
	void updateEffectsToxicity();

	Potions m_allPotions, m_filteredPotions;
	Filters m_currentFilters;
	Callback m_updateCallback;
	std::vector<bool> m_toxicity;
};

//****************************************************************************//

inline const PotionsList::Potions& PotionsList::allPotions() const
{ return m_allPotions; }

inline const PotionsList::Potions& PotionsList::filteredPotions() const
{ return m_filteredPotions; }

inline const PotionsList::Filters& PotionsList::filters() const
{ return m_currentFilters; }

inline void PotionsList::setUpdateCallback(Callback func)
{ m_updateCallback = func; }

#endif // POTIONSLIST_H
