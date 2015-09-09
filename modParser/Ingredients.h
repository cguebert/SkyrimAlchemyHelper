#pragma once

#include <string>
#include <cstdint>
#include <vector>

class Ingredients
{
public:
	struct EffectData
	{
		uint32_t id = 0;
		float magnitude = 0;
		uint32_t duration = 0;
	};

	struct Ingredient
	{
		uint32_t id = 0;
		std::string name, modName;
		EffectData effects[4];
	};

	bool setIngredient(const Ingredient& ingredient); // return true if adding, false if modifying existing
	const std::vector<Ingredient>& ingredients() const;

protected:
	std::vector<Ingredient> m_ingredients;
};

