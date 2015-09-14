#pragma once

#include <QWidget>
#include <QAbstractTableModel>

class QTableView;
class Config;

class IngredientsListModel : public QAbstractTableModel
{
public:
	IngredientsListModel(Config& config, QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	void beginReset();
	void endReset();

protected:
	Config& m_config;
};

//****************************************************************************//

class IngredientsListWidget : public QWidget
{
	Q_OBJECT
public:
	explicit IngredientsListWidget(Config& config, QWidget* parent = nullptr);

public slots:
	void beginReset();
	void endReset();

protected:
	QTableView *m_view;
	IngredientsListModel *m_model;
};
