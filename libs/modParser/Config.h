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
*                      Skyrim Alchemy Helper :: ModParser                     *
*                                                                             *
*                  Contact: christophe.guebert+SAH@gmail.com                  *
******************************************************************************/
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

//****************************************************************************//

struct Container 
{ 
	uint32_t id = 0, cell = 0; 
	std::string type, location; 
	bool interior = true; 
};
using Containers = std::vector<Container>;

} // namespace modParser
