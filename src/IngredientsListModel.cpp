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

bool IngredientsListModel::changeEffect(int ingredientIndex, int effectIndex, QString effectName)
{
	auto& ingredient = m_ingredientsList.ingredient(ingredientIndex);
	auto& effect = ingredient.effects[effectIndex];

	auto oldEffectId = effect.effectId;
	auto newEffectId = m_effectsList.find(effectName);
	if(newEffectId != -1 && oldEffectId != newEffectId)
	{
		m_effectsList.decrementNbIngredients(oldEffectId);
		m_effectsList.incrementNbIngredients(newEffectId);
		effect.effectId = newEffectId;
		return true;
	}

	return false;
}

bool IngredientsListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::EditRole)
	{
		auto& ingredient = m_ingredientsList.ingredient(index.row());
		bool ok = false;
		switch (index.column())
		{
		case 0:
			ingredient.name = value.toString();
			return true;
		case 1:
			return -1 != (ingredient.pluginId = m_pluginsList.find(value.toString()));
		case 2:
			return changeEffect(index.row(), 0, value.toString());
		case 3:
			ingredient.effects[0].magnitude = value.toFloat(&ok);
			return ok;
		case 4:
			ingredient.effects[0].duration = value.toFloat(&ok);
			return ok;
		case 5:
			return changeEffect(index.row(), 1, value.toString());
		case 6:
			ingredient.effects[1].magnitude = value.toFloat(&ok);
			return ok;
		case 7:
			ingredient.effects[1].duration = value.toFloat(&ok);
			return ok;
		case 8:
			return changeEffect(index.row(), 2, value.toString());
		case 9:
			ingredient.effects[2].magnitude = value.toFloat(&ok);
			return ok;
		case 10:
			ingredient.effects[2].duration = value.toFloat(&ok);
			return ok;
		case 11:
			return changeEffect(index.row(), 3, value.toString());
		case 12:
			ingredient.effects[3].magnitude = value.toFloat(&ok);
			return ok;
		case 13:
			ingredient.effects[3].duration = value.toFloat(&ok);
			return ok;
		}
	}
	return false;
}

Qt::ItemFlags IngredientsListModel::flags(const QModelIndex& index) const
{
	return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

bool IngredientsListModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows(parent, row, row + count - 1);
	m_ingredientsList.removeIngredients(row, count);
	endRemoveRows();

	return true;
}

bool IngredientsListModel::insertRows(int row, int count, const QModelIndex& parent)
{
	beginInsertRows(parent, row, row + count - 1);
	m_ingredientsList.addIngredient();
	endInsertRows();
	return true;
}
