#include "PluginsListModel.h"
#include "PluginsList.h"

#include <QInputDialog>

PluginsListModel::PluginsListModel(QObject* parent)
	: QAbstractTableModel(parent)
	, m_pluginsList(PluginsList::GetInstance())
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

bool PluginsListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if(index.column() == 0 && role == Qt::EditRole)
	{
		m_pluginsList.plugin(index.row()).name = value.toString();
		return true;
	}
	if(index.column() == 1 && role == Qt::CheckStateRole)
	{
		bool active = value.toBool();
		m_pluginsList.setActive(index.row(), active);
		return true;
	}
	return false;
}

Qt::ItemFlags PluginsListModel::flags(const QModelIndex& index) const
{
	if(index.column() == 0)
		return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
	if(index.column() == 1)
		return Qt::ItemIsUserCheckable | QAbstractTableModel::flags(index);
	return QAbstractTableModel::flags(index);
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

bool PluginsListModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows(parent, row, row + count - 1);
	m_pluginsList.removePlugins(row, count);
	endRemoveRows();

	return true;
}

bool PluginsListModel::insertRows(int row, int count, const QModelIndex& parent)
{
	beginInsertRows(parent, row, row + count - 1);
	m_pluginsList.addPlugin();
	endInsertRows();
	return true;
}
