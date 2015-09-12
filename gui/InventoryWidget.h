#pragma once

#include <QWidget>
#include <QAbstractTableModel>

class QTableView;
class GameSave;
class IngredientsList;

class InventoryModel : public QAbstractTableModel
{
public:
	InventoryModel(const std::vector<std::pair<int, int>>& inventory, QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	void beginReset();
	void endReset();

protected:
	IngredientsList& m_ingredientsList;
	const std::vector<std::pair<int, int>>& m_inventory;
};

//****************************************************************************//

class InventoryWidget : public QWidget
{
	Q_OBJECT
public:
	explicit InventoryWidget(const std::vector<std::pair<int, int>>& inventory, QWidget* parent = nullptr);

public slots:
	void beginReset();
	void endReset();

protected:
	QTableView *m_view;
	InventoryModel *m_model;
};
