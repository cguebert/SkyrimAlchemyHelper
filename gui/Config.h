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
#pragma once

#include <QString>
#include <vector>
#include <memory>
#include <mutex>

namespace modParser { struct Config; }

class Config
{
public:
	struct Plugin
	{
		Plugin(QString name = "") : name(name) {}
		int nbIngredients = 0;
		QString name;
	};
	using Plugins = std::vector<Plugin>;

	struct Effect
	{
		uint32_t code = 0, flags = 0;
		float baseCost = 0;
		QString name, description, tooltip;
		std::vector<size_t> ingredients;
	};
	using Effects = std::vector<Effect>;

	static const int nbEffectsPerIngredient = 4;

	struct EffectData
	{
		int effectId = -1;
		float magnitude = 0, duration = 0;
	};

	struct Ingredient
	{
		int pluginId = 0;
		uint32_t code = 0;
		QString name, tooltip;
		EffectData effects[nbEffectsPerIngredient],
			sortedEffects[nbEffectsPerIngredient];
	};
	using Ingredients = std::vector<Ingredient>;

public:
	static Config& main() // Not a singleton, just the one everyone use
	{ static Config config; return config; }

	Plugins plugins;
	Effects effects;
	Ingredients ingredients;

	void load();
	void save() const;

	int indexOfPlugin(QString name) const;
	int indexOfEffect(uint32_t id) const;
	int indexOfIngredient(uint32_t pluginId, uint32_t ingredientId) const;

protected:
	void loadPlugins();
	void loadEffects();
	void loadIngredients();

	void savePlugins() const;
	void saveEffects() const;
	void saveIngredients() const;
};

//****************************************************************************//

class ContainersCache
{
public:
	static ContainersCache& instance()
	{ static ContainersCache cache; return cache; }

	struct Container
	{
		uint32_t code = 0, cellCode = 0;
		bool interior = true;
		QString name, location;
	};
	using Containers = std::vector<Container>;
	Containers containers;

	std::mutex containersMutex; // Because other threads will update it

	void load();
	void save();

protected:
	ContainersCache() = default;
};
