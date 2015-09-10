#ifndef INGREDIENTSLIST_H
#define INGREDIENTSLIST_H

#include <QString>
#include <QStringList>
#include <vector>

class IngredientsList
{
public:
	static IngredientsList& instance();

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

	int size() const;
	int find(int pluginId, quint32 code) const; // -1 if not found
	const std::vector<Ingredient>& ingredients() const;
	std::vector<Ingredient>& ingredients();

	void loadList();
	void saveList();

protected:
	IngredientsList();

	std::vector<Ingredient> m_ingredients;
};

//****************************************************************************//

inline int IngredientsList::size() const
{ return m_ingredients.size(); }

inline const std::vector<IngredientsList::Ingredient>& IngredientsList::ingredients() const
{ return m_ingredients; }

inline std::vector<IngredientsList::Ingredient>& IngredientsList::ingredients()
{ return m_ingredients; }

#endif // INGREDIENTSLIST_H
