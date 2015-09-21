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
		quint32 code = 0, flags = 0;
		float baseCost = 0;
		QString name, description, tooltip;
		std::vector<int> ingredients;
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
		quint32 code = 0;
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
	int indexOfEffect(quint32 id) const;
	int indexOfIngredient(quint32 pluginId, quint32 ingredientId) const;

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
		quint32 code = 0, cellCode = 0;
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
