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

	void beginReset();
	void endReset();

protected:
	IngredientsList& m_ingredientsList;
	EffectsList& m_effectsList;
	PluginsList& m_pluginsList;
};

#endif // INGREDIENTSLISTMODEL_H
