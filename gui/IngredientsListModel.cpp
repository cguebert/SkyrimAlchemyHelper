#include "IngredientsListModel.h"
#include "IngredientsList.h"
#include "EffectsList.h"
#include "PluginsList.h"

IngredientsListModel::IngredientsListModel(QObject* parent)
	: QAbstractTableModel(parent)
	, m_ingredientsList(IngredientsList::GetInstance())
	, m_effectsList(EffectsList::GetInstance())
	, m_pluginsList(PluginsList::GetInstance())
{
}

int IngredientsListModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_ingredientsList.size();
}

int IngredientsListModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 14;
}

QVariant IngredientsListModel::data(const QModelIndex& index, int role) const
{
	if(role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const auto& ingredient = m_ingredientsList.ingredient(index.row());
		switch (index.column())
		{
		case 0: return ingredient.name;
		case 1:
			return ingredient.pluginId == -1 ? "" : m_pluginsList.plugin(ingredient.pluginId).name;
		case 2:
		{
			const auto id = ingredient.effects[0].effectId;
			return id == -1 ? "" : m_effectsList.effect(id).name;
		}
		case 3: return ingredient.effects[0].magnitude;
		case 4: return ingredient.effects[0].duration;
		case 5:
		{
			const auto id = ingredient.effects[1].effectId;
			return id == -1 ? "" : m_effectsList.effect(id).name;
		}
		case 6: return ingredient.effects[1].magnitude;
		case 7: return ingredient.effects[1].duration;
		case 8:
		{
			const auto id = ingredient.effects[2].effectId;
			return id == -1 ? "" : m_effectsList.effect(id).name;
		}
		case 9: return ingredient.effects[2].magnitude;
		case 10: return ingredient.effects[2].duration;
		case 11:
		{
			const auto id = ingredient.effects[3].effectId;
			return id == -1 ? "" : m_effectsList.effect(id).name;
		}
		case 12: return ingredient.effects[3].magnitude;
		case 13: return ingredient.effects[3].duration;
		}
		return QVariant();
	}
	return QVariant();
}

QVariant IngredientsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0: return QString("name");
		case 1: return QString("plugin");
		case 2: return QString("ingredient 1");
		case 3: return QString("magnitude 1");
		case 4: return QString("duration 1");
		case 5: return QString("ingredient 2");
		case 6: return QString("magnitude 2");
		case 7: return QString("duration 2");
		case 8: return QString("ingredient 3");
		case 9: return QString("magnitude 3");
		case 10: return QString("duration 3");
		case 11: return QString("ingredient 4");
		case 12: return QString("magnitude 4");
		case 13: return QString("duration 4");
		}
	}
	return QVariant();
}
