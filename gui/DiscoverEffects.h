#pragma once

#include <array>
#include <vector>

class DiscoverEffects
{
public:
	static std::vector<int> selectPotions(int nb = 20);

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
	std::vector<int> m_selectedPotions;
	std::vector<std::array<bool, 4>> m_knownIngredients;
};
