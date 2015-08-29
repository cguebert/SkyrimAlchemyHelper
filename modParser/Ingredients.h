#pragma once

#include <string>
#include <cstdint>
#include <vector>

using MagicalEffect = std::pair < uint32_t, std::string >;
using MagicalEffectsList = std::vector < MagicalEffect >;

class Ingredients
{
public:
	struct Effect
	{
		uint32_t id;
		float magnitude;
		uint32_t duration;
	};

	struct Ingredient
	{
		uint32_t id;
		std::string name, modName;
		Effect effects[4];
	};

	bool setIngredient(const Ingredient& ingredient); // return true if adding, false if modifying existing
	const std::vector<Ingredient>& ingredients() const;

protected:
	std::vector<Ingredient> m_ingredients;
};

