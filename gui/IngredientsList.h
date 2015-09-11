#ifndef INGREDIENTSLIST_H
#define INGREDIENTSLIST_H

#include <QString>
#include <vector>

class EffectsList;
class PluginsList;

class IngredientsList
{
public:
	static IngredientsList& instance() // Not a singleton, just the one everyone use
	{ static IngredientsList ingredients; return ingredients; }

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

	int size() const;
	int find(int pluginId, quint32 code) const; // -1 if not found
	const Ingredients& ingredients() const;
	Ingredients& ingredients();

	void loadList(EffectsList* effectsListPtr = nullptr, PluginsList* pluginsListPtr = nullptr);
	void saveList();

protected:
	Ingredients m_ingredients;
};

//****************************************************************************//

inline int IngredientsList::size() const
{ return m_ingredients.size(); }

inline const IngredientsList::Ingredients& IngredientsList::ingredients() const
{ return m_ingredients; }

inline IngredientsList::Ingredients& IngredientsList::ingredients()
{ return m_ingredients; }

#endif // INGREDIENTSLIST_H
