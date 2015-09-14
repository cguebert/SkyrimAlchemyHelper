#pragma once

#include <QWidget>
#include <QAbstractTableModel>

class QTableView;
class GameSave;

class KnownIngredientsModel : public QAbstractTableModel
{
public:
	KnownIngredientsModel(GameSave& gamesave, QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	void beginReset();
	void endReset();

protected:
	GameSave& m_gameSave;
};

//****************************************************************************//

class KnownIngredientsWidget : public QWidget
{
	Q_OBJECT
public:
	explicit KnownIngredientsWidget(GameSave& gamesave, QWidget* parent = nullptr);

public slots:
	void beginReset();
	void endReset();

protected:
	QTableView *m_view;
	KnownIngredientsModel *m_model;
};
