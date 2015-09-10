#ifndef INGREDIENTSLIST_H
#define INGREDIENTSLIST_H

#include <QString>
#include <QVector>

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
		QString name;
		int pluginId = 0;
		quint32 code = 0;
		EffectData effects[nbEffectsPerIngredient],
			sortedEffects[nbEffectsPerIngredient];
	};

	int size() const;
	int find(int pluginId, quint32 code) const; // -1 if not found
	const QVector<Ingredient>& ingredients() const;
	QVector<Ingredient>& ingredients();

	void loadList();
	void saveList();

protected:
	IngredientsList();

	QVector<Ingredient> m_ingredients;
};

//****************************************************************************//

inline int IngredientsList::size() const
{ return m_ingredients.size(); }

inline const QVector<IngredientsList::Ingredient>& IngredientsList::ingredients() const
{ return m_ingredients; }

inline QVector<IngredientsList::Ingredient>& IngredientsList::ingredients()
{ return m_ingredients; }

#endif // INGREDIENTSLIST_H
