#include "PluginsListModel.h"
#include "PluginsList.h"

#include <QInputDialog>

PluginsListModel::PluginsListModel(QObject* parent)
	: QAbstractTableModel(parent)
	, m_pluginsList(PluginsList::instance())
{

}

int PluginsListModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_pluginsList.size();
}


int PluginsListModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 3;
}

QVariant PluginsListModel::data(const QModelIndex& index, int role) const
{
	switch(role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
		{
			const auto& plugin = m_pluginsList.plugin(index.row());
			switch (index.column())
			{
				case 0: return plugin.name;
				case 1: return plugin.active;
				case 2: return plugin.nbIngredients;
			}
			return QVariant();
		}
		case Qt::CheckStateRole:
		{
			if(index.column() == 1)
			{
				const auto& plugin = m_pluginsList.plugin(index.row());
				return plugin.active ? Qt::Checked : Qt::Unchecked;
			}
			return QVariant();
		}
	}
	return QVariant();
}

QVariant PluginsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case 0: return QString("name");
			case 1: return QString("active");
			case 2: return QString("# ingredients");
		}
	}
	return QVariant();
}

void PluginsListModel::beginReset()
{
	beginResetModel();
}

void PluginsListModel::endReset()
{
	endResetModel();
}
