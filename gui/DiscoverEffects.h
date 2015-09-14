#pragma once

#include "PotionsList.h"

#include <array>

class DiscoverEffects
{
public:
	static std::pair<PotionsList::PotionsId, PotionsList::PotionAdditionalDataList> selectPotions(int nb = 20);

protected:
	DiscoverEffects(int nbPotions);
	void doComputation();

	void setFilters();
	void setSortingFunction();
	bool selectOnePotion();

	void prepare();
	void cleanup();

	int m_nbPotions;
	std::vector<int> m_ingredientsCount;
	std::vector<std::array<bool, 4>> m_knownIngredients;
	PotionsList::PotionsId m_selectedPotions;
	PotionsList::PotionAdditionalDataList m_additionalData;
};
