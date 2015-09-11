#pragma once

#include <QWidget>
#include <QAbstractTableModel>

class QPushButton;
class QTableView;
class EffectsList;

class EffectsListModel : public QAbstractTableModel
{
public:
	EffectsListModel(EffectsList& effectsList, QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	void beginReset();
	void endReset();

protected:
	EffectsList& m_effectsList;
};

//****************************************************************************//

class EffectsListWidget : public QWidget
{
	Q_OBJECT
public:
	explicit EffectsListWidget(EffectsList& effectsList, QWidget* parent = nullptr);

public slots:
	void beginReset();
	void endReset();

protected:
	QTableView *m_view;
	EffectsListModel *m_model;
};
