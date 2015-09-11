#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace modParser
{

using ModsList = std::vector < std::string >;

struct MagicalEffect
{
	uint32_t id = 0, flags = 0;
	std::string name, description;
	float baseCost = 0;
};
using MagicalEffectsList = std::vector < MagicalEffect >;

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
using IngredientsList = std::vector<Ingredient>;

struct Config
{
	IngredientsList ingredientsList;
	MagicalEffectsList magicalEffectsList;
	ModsList modsList;
};

} // namespace modParser
