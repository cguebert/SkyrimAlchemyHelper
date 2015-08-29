#include <QFile>
#include <QTextStream>

#include "IngredientsList.h"
#include "EffectsList.h"
#include "PluginsList.h"

const QString fileName = "data/ingredients.txt";

IngredientsList& IngredientsList::GetInstance()
{
	static IngredientsList ingredients;
	return ingredients;
}

IngredientsList::IngredientsList()
{
	loadList();
}

void IngredientsList::loadList()
{
	PluginsList& plugins = PluginsList::GetInstance();
	EffectsList& effects = EffectsList::GetInstance();

	QFile inputFile(fileName);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			Ingredient ingredient;
			ingredient.name = in.readLine();

			QString pluginName = in.readLine();
			ingredient.pluginId = plugins.find(pluginName);

			QString ingredientId = in.readLine();
			ingredient.ingId = ingredientId.toUInt(nullptr, 16);

			if (ingredient.pluginId != -1)
				plugins.incrementNbIngredients(ingredient.pluginId);

			bool validEffects = true;
			for (int i = 0; i < 4; ++i)
			{
				EffectData& effectData = ingredient.effects[i];
				QString line = in.readLine();
				QStringList split = line.split(" ");
				if (split.size() == 3)
				{
					effectData.effectId = effects.find(split[0].toUInt(nullptr, 16));
					effectData.magnitude = split[1].toFloat();
					effectData.duration = split[2].toFloat();
				}

				if (effectData.effectId == -1)
					validEffects = false;
				else
					effects.incrementNbIngredients(effectData.effectId);
			}

			if (!ingredient.name.isEmpty() && ingredient.pluginId != -1 && validEffects)
				m_ingredients.push_back(ingredient);
		}
		inputFile.close();
	}

	// Sort the list by name
	std::sort(m_ingredients.begin(), m_ingredients.end(), [](const Ingredient& lhs, const Ingredient& rhs){
		return lhs.name < rhs.name;
	});
}

void IngredientsList::saveList()
{
	PluginsList& plugins = PluginsList::GetInstance();
	EffectsList& effects = EffectsList::GetInstance();

	QFile outputFile(fileName);
	if(outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&outputFile);
		for(const Ingredient& ingredient : m_ingredients)
		{
			out << ingredient.name << '\n';
			if(ingredient.pluginId != -1)
				out << plugins.plugin(ingredient.pluginId).name;
			out << '\n';
			out << QString::number(ingredient.ingId, 16).toUpper() << '\n';

			for(int i = 0; i < 4; ++i)
			{
				const EffectData& effectData = ingredient.effects[i];
				const EffectsList::Effect& effect = effects.effect(effectData.effectId);
				out << QString::number(effect.id, 16).toUpper() << ' ';
				out << effectData.magnitude << ' ' << effectData.duration << '\n';
			}
		}
	}
}

int IngredientsList::find(QString name) const
{
	auto it = std::find_if(m_ingredients.begin(), m_ingredients.end(), [&name](const Ingredient& ingredient){
		return !ingredient.name.compare(name, Qt::CaseInsensitive);
	});
	if(it != m_ingredients.end())
		return it - m_ingredients.begin();
	return -1;
}

void IngredientsList::removeIngredients(int first, int count)
{
	static EffectsList& effects = EffectsList::GetInstance();
	for(int i = first, nb = m_ingredients.size(); i < nb && i < first + count; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			int id = m_ingredients[i].effects[j].effectId;
			if(id != -1)
				effects.decrementNbIngredients(id);
		}
	}

	auto start = m_ingredients.begin() + first;
	auto end = start + count;
	m_ingredients.erase(start, end);
}

void IngredientsList::addIngredient()
{
	Ingredient ingredient;
	ingredient.name = "New Ingredient";
	m_ingredients.push_back(ingredient);
}
