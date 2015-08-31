#include "EffectsListModel.h"
#include "EffectsList.h"

EffectsListModel::EffectsListModel(QObject* parent)
	: QAbstractTableModel(parent)
	, m_effectsList(EffectsList::GetInstance())
{
}

int EffectsListModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_effectsList.size();
}

int EffectsListModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 3;
}

QVariant EffectsListModel::data(const QModelIndex& index, int role) const
{
	if(role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const auto& effect = m_effectsList.effect(index.row());
		switch (index.column())
		{
			case 0: return effect.name;
			case 1: return QString::number(effect.id, 16).toUpper();
			case 2: return effect.nbIngredients;
		}
		return QVariant();
	}
	return QVariant();
}

QVariant EffectsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case 0: return QString("name");
			case 1: return QString("id");
			case 2: return QString("# ingredients");
		}
	}
	return QVariant();
}

void EffectsListModel::beginReset()
{
	beginResetModel();
}

void EffectsListModel::endReset()
{
	endResetModel();
}
