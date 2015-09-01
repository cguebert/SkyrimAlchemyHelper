#include "PotionsListModel.h"
#include "PotionsList.h"

PotionsListModel::PotionsListModel(QObject* parent)
	: QAbstractTableModel(parent)
	, m_potionsList(PotionsList::instance())
{
}

int PotionsListModel::rowCount(const QModelIndex& /*parent*/) const
{
	return 5;
}

int PotionsListModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 3;
}

QVariant PotionsListModel::data(const QModelIndex& index, int role) const
{
	if(role == Qt::DisplayRole)
	{
		if(index.row() == 0 && index.column() == 0)
		{
			return "toto\ntiti\ntata";
		}
		else if(index.row() == 0 && index.column() == 2)
		{
			return "Fortify Destruction\nFortify Sneak\nLingering Damage Health\nDamage Health";
		}
		return QVariant();
	}
	return QVariant();
}

QVariant PotionsListModel::headerData(int section, Qt::Orientation orientation, int role) const
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
