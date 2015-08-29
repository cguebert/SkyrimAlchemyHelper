#ifndef PLUGINSLISTMODEL_H
#define PLUGINSLISTMODEL_H

#include <QAbstractTableModel>

class PluginsList;

class PluginsListModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	PluginsListModel(QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	bool removeRows(int row, int count, const QModelIndex& parent) override;
	bool insertRows(int row, int count, const QModelIndex& parent) override;

protected:
	PluginsList& m_pluginsList;
};

#endif // PLUGINSLISTMODEL_H
