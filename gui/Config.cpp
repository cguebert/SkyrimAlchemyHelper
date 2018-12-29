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
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "Config.h"
#include <modParser/ModParser.h>

namespace
{

const QString pluginsFileName = "data/plugins.txt";
const QString effectsFileName = "data/effects.txt";
const QString ingredientsFileName = "data/ingredients.txt";
const QString containersFileName = "data/containers.txt";

QString convert(const std::string& text)
{
	return QString::fromLatin1(text.c_str());
}

}

void Config::load()
{
	loadPlugins();
	loadEffects();
	loadIngredients();
}

void Config::loadPlugins()
{
	plugins.clear();
	QFile inputFile(pluginsFileName);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			Plugin plugin;
			plugin.name = in.readLine();
			if (!plugin.name.isEmpty())
				plugins.push_back(plugin);
		}
		inputFile.close();
	}

	// Sort the list by name
	std::sort(plugins.begin(), plugins.end(), [](const Plugin& lhs, const Plugin& rhs){
		return lhs.name < rhs.name;
	});
}

void Config::loadEffects()
{
	effects.clear();
	QFile inputFile(effectsFileName);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			Effect effect;
			effect.name = in.readLine();

			QString line = in.readLine();
			QStringList split = line.split(" ");
			if (split.size() == 3)
			{
				effect.code = split[0].toUInt(nullptr, 16);
				effect.flags = split[1].toUInt(nullptr, 16);
				effect.baseCost = split[2].toFloat();
			}

			effect.description = in.readLine();
			if (!effect.name.isEmpty() && effect.code)
				effects.push_back(effect);
		}
		inputFile.close();
	}

	// Sort the list by name
	std::sort(effects.begin(), effects.end(), [](const Effect& lhs, const Effect& rhs){
		return lhs.name < rhs.name;
	});
}

void Config::loadIngredients()
{
	ingredients.clear();

	QFile inputFile(ingredientsFileName);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			Ingredient ingredient;
			ingredient.name = in.readLine();

			QString pluginName = in.readLine();
			ingredient.pluginId = indexOfPlugin(pluginName);

			QString ingredientId = in.readLine();
			ingredient.code = ingredientId.toUInt(nullptr, 16);

			if (ingredient.pluginId != -1)
				++plugins[ingredient.pluginId].nbIngredients;

			bool validEffects = true;
			for (int i = 0; i < 4; ++i)
			{
				EffectData& effectData = ingredient.effects[i];
				QString line = in.readLine();
				QStringList split = line.split(" ");
				if (split.size() == 3)
				{
					effectData.effectId = indexOfEffect(split[0].toUInt(nullptr, 16));
					effectData.magnitude = split[1].toFloat();
					effectData.duration = split[2].toFloat();
				}

				if (effectData.effectId == -1)
				{
					validEffects = false;
					break;
				}
			}

			if (!ingredient.name.isEmpty() && ingredient.pluginId != -1 && validEffects)
			{
				// Sort the effects for an easier computation of potions,
				//  but keep the original ones as they are for the known ingredients loaded for each save
				std::copy(std::begin(ingredient.effects), std::end(ingredient.effects), std::begin(ingredient.sortedEffects));
				std::sort(std::begin(ingredient.sortedEffects), std::end(ingredient.sortedEffects), [](const EffectData& lhs, const EffectData& rhs){
					return lhs.effectId < rhs.effectId;
				});

				ingredients.push_back(ingredient);
			}
		}
		inputFile.close();
	}

	// Sort the list by name
	sort(ingredients.begin(), ingredients.end(), [](const Ingredient& lhs, const Ingredient& rhs){
		return lhs.name < rhs.name;
	});

	// Create the tooltips of the effects (with the sorted list of ingredients)
	for (size_t i = 0, nb = ingredients.size(); i < nb; ++i)
	{
		auto& ingredient = ingredients[i];
		for (auto effectData : ingredient.sortedEffects)
		{
			auto& effect = effects[effectData.effectId];
			ingredient.tooltip += effect.name + "\n";
			effect.ingredients.push_back(i);
			effect.tooltip += ingredient.name + "\n";
		}
		ingredient.tooltip = ingredient.tooltip.trimmed();
	}

	// Trim the tooltips of the effects
	for (auto& effect : effects)
		effect.tooltip = effect.tooltip.trimmed();
}

void Config::save() const
{
	savePlugins();
	saveEffects();
	saveIngredients();
}

void Config::savePlugins() const
{
	QFile outputFile(pluginsFileName);
	if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&outputFile);
		for (const Plugin& plugin : plugins)
			out << plugin.name << '\n';
	}
}

void Config::saveEffects() const
{
	QFile outputFile(effectsFileName);
	if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&outputFile);
		for (const Effect& effect : effects)
		{
			out << effect.name << '\n';
			out << QString::number(effect.code, 16).toUpper() << ' ';
			out << QString::number(effect.flags, 16).toUpper() << ' ';
			out << effect.baseCost << '\n';
			out << effect.description << '\n';
		}
	}
}

void Config::saveIngredients() const
{
	QFile outputFile(ingredientsFileName);
	if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&outputFile);
		for (const Ingredient& ingredient : ingredients)
		{
			out << ingredient.name << '\n';
			if (ingredient.pluginId != -1)
				out << plugins[ingredient.pluginId].name;
			out << '\n';
			out << QString::number(ingredient.code, 16).toUpper() << '\n';

			for (int i = 0; i < 4; ++i)
			{
				const EffectData& effectData = ingredient.effects[i];
				const Effect& effect = effects[effectData.effectId];
				out << QString::number(effect.code, 16).toUpper() << ' ';
				out << effectData.magnitude << ' ' << effectData.duration << '\n';
			}
		}
	}
}

int Config::indexOfPlugin(QString name) const
{
	auto it = std::find_if(plugins.begin(), plugins.end(), [&name](const Config::Plugin& plugin){
		return !plugin.name.compare(name, Qt::CaseInsensitive);
	});
	if (it != plugins.end())
		return it - plugins.begin();
	return -1;
}

int Config::indexOfEffect(uint32_t id) const
{
	auto it = std::find_if(effects.begin(), effects.end(), [&id](const Config::Effect& effect){
		return effect.code == id;
	});
	if (it != effects.end())
		return it - effects.begin();
	return -1;
}

int Config::indexOfIngredient(uint32_t pluginId, uint32_t ingredientId) const
{
	auto it = find_if(ingredients.begin(), ingredients.end(), [pluginId, ingredientId](const Ingredient& ingredient){
		return ingredient.pluginId == pluginId && ingredient.code == ingredientId;
	});
	if (it != ingredients.end())
		return it - ingredients.begin();
	return -1;
}

//****************************************************************************//

void ContainersCache::load()
{
	Containers containersList;
	QFile inputFile(containersFileName);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			Container container;
			QString line = in.readLine();
			QStringList split = line.split(" ");
			if (split.size() == 3)
			{
				container.code = split[0].toUInt(nullptr, 16);
				container.cellCode = split[1].toUInt(nullptr, 16);
				container.interior = (split[2] !=  "0");
			}

			container.name = in.readLine();
			container.location = in.readLine();
			if (!container.name.isEmpty() && !container.location.isEmpty())
				containersList.push_back(container);
		}
		inputFile.close();
	}

	{
		std::lock_guard<std::mutex> lock(containersMutex);
		containers = containersList;
	}
}

void ContainersCache::save()
{
	ContainersCache::Containers containersList;
	{
		std::lock_guard<std::mutex> lock(containersMutex);
		containersList = containers;
	}

	QFile outputFile(containersFileName);
	if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&outputFile);
		for (const auto& container : containersList)
		{
			out << QString::number(container.code, 16).toUpper() << ' ';
			out << QString::number(container.cellCode, 16).toUpper() << ' ';
			out << (container.interior ? '1' : '0') << '\n';
			out << container.name << '\n';
			out << container.location << '\n';
		}
	}
}
