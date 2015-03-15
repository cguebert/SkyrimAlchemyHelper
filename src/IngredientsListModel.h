#ifndef INGREDIENTSLISTMODEL_H
#define INGREDIENTSLISTMODEL_H

#include <QAbstractTableModel>

class IngredientsList;
class EffectsList;
class PluginsList;

class IngredientsListModel : public QAbstractTableModel
{
public:
	IngredientsListModel(QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	bool removeRows(int row, int count, const QModelIndex& parent) override;
	bool insertRows(int row, int count, const QModelIndex& parent) override;

protected:
	bool changeEffect(int ingredientIndex, int effectIndex, QString effectName);

	IngredientsList& m_ingredientsList;
	EffectsList& m_effectsList;
	PluginsList& m_pluginsList;
};

#endif // INGREDIENTSLISTMODEL_H
