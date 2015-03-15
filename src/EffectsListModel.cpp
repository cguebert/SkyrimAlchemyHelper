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
	switch(role)
	{
		case Qt::DisplayRole:
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

bool EffectsListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if(role == Qt::EditRole)
	{
		switch(index.column())
		{
		case 0:
			m_effectsList.effect(index.row()).name = value.toString();
			return true;
		case 1:
			m_effectsList.effect(index.row()).id = value.toString().toUInt(nullptr, 16);
			return true;
		}
	}
	return false;
}

Qt::ItemFlags EffectsListModel::flags(const QModelIndex& index) const
{
	if(index.column() == 0 || index.column() == 1)
		return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
	return QAbstractTableModel::flags(index);
}

bool EffectsListModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows(parent, row, row + count - 1);
	m_effectsList.removeEffects(row, count);
	endRemoveRows();

	return true;
}

bool EffectsListModel::insertRows(int row, int count, const QModelIndex& parent)
{
	beginInsertRows(parent, row, row + count - 1);
	m_effectsList.addEffect();
	endInsertRows();
	return true;
}
