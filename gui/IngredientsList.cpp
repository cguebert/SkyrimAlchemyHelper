#include <QFile>
#include <QTextStream>

#include "IngredientsList.h"
#include "EffectsList.h"
#include "PluginsList.h"

using namespace std;

const QString fileName = "data/ingredients.txt";

void IngredientsList::loadList(EffectsList* effectsListPtr, PluginsList* pluginsListPtr)
{
	m_ingredients.clear();

	EffectsList& effectsList = effectsListPtr ? *effectsListPtr : EffectsList::instance();
	PluginsList& pluginsList = pluginsListPtr ? *pluginsListPtr : PluginsList::instance();

	QFile inputFile(fileName);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			Ingredient ingredient;
			ingredient.name = in.readLine();

			QString pluginName = in.readLine();
			ingredient.pluginId = pluginsList.find(pluginName);

			QString ingredientId = in.readLine();
			ingredient.code = ingredientId.toUInt(nullptr, 16);

			if (ingredient.pluginId != -1)
				++pluginsList.plugins()[ingredient.pluginId].nbIngredients;

			bool validEffects = true;
			for (int i = 0; i < 4; ++i)
			{
				EffectData& effectData = ingredient.effects[i];
				QString line = in.readLine();
				QStringList split = line.split(" ");
				if (split.size() == 3)
				{
					effectData.effectId = effectsList.find(split[0].toUInt(nullptr, 16));
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
				copy(begin(ingredient.effects), end(ingredient.effects), begin(ingredient.sortedEffects));
				sort(begin(ingredient.sortedEffects), end(ingredient.sortedEffects), [](const EffectData& lhs, const EffectData& rhs){
					return lhs.effectId < rhs.effectId;
				});

				m_ingredients.push_back(ingredient);
			}
		}
		inputFile.close();
	}

	// Sort the list by name
	sort(m_ingredients.begin(), m_ingredients.end(), [](const Ingredient& lhs, const Ingredient& rhs){
		return lhs.name < rhs.name;
	});

	// Create the tooltips of the effects (with the sorted list of ingredients)
	for (int i = 0, nb = m_ingredients.size(); i < nb; ++i)
	{
		auto& ingredient = m_ingredients[i];
		for (auto effectData : ingredient.sortedEffects)
		{
			auto& effect = effectsList.effects()[effectData.effectId];
			ingredient.tooltip += effect.name + "\n";
			effect.ingredients.push_back(i);
			effect.tooltip += ingredient.name + "\n";
		}
		ingredient.tooltip = ingredient.tooltip.trimmed();
	}

	// Trim the tooltips of the effects
	for (auto& effect : effectsList.effects())
		effect.tooltip = effect.tooltip.trimmed();
}

void IngredientsList::saveList()
{
	PluginsList& pluginsList = PluginsList::instance();
	EffectsList& effectsList = EffectsList::instance();

	QFile outputFile(fileName);
	if(outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&outputFile);
		for(const Ingredient& ingredient : m_ingredients)
		{
			out << ingredient.name << '\n';
			if(ingredient.pluginId != -1)
				out << pluginsList.plugins()[ingredient.pluginId].name;
			out << '\n';
			out << QString::number(ingredient.code, 16).toUpper() << '\n';

			for(int i = 0; i < 4; ++i)
			{
				const EffectData& effectData = ingredient.effects[i];
				const EffectsList::Effect& effect = effectsList.effects()[effectData.effectId];
				out << QString::number(effect.code, 16).toUpper() << ' ';
				out << effectData.magnitude << ' ' << effectData.duration << '\n';
			}
		}
	}
}

int IngredientsList::find(int pluginId, quint32 code) const
{
	auto it = find_if(m_ingredients.begin(), m_ingredients.end(), [pluginId, code](const Ingredient& ingredient){
		return ingredient.pluginId == pluginId && ingredient.code == code;
	});
	if (it != m_ingredients.end())
		return it - m_ingredients.begin();
	return -1;
}
