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
	int find(quint32 code) const; // -1 if not found
	int find(QString name) const; // -1 if not found
	const QVector<Ingredient>& ingredients() const;
	Ingredient& ingredient(int index); // Will launch an exception if index is invalid
	const Ingredient& ingredient(int index) const; // Will launch an exception if index is invalid

	void removeIngredients(int first, int count = 1);
	void addIngredient();

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

inline IngredientsList::Ingredient& IngredientsList::ingredient(int index)
{ return m_ingredients[index]; }

inline const IngredientsList::Ingredient& IngredientsList::ingredient(int index) const
{ return m_ingredients[index]; }

#endif // INGREDIENTSLIST_H
