#pragma once

#include <QWidget>
#include <QAbstractTableModel>

class QPushButton;
class QTableView;
class PluginsList;

class PluginsListModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	PluginsListModel(QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	void beginReset();
	void endReset();

protected:
	PluginsList& m_pluginsList;
};

//****************************************************************************//

class PluginsListWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PluginsListWidget(QWidget* parent = nullptr);

public slots:
	void beginReset();
	void endReset();

protected:
	QTableView *m_view;
	PluginsListModel *m_model;
};
