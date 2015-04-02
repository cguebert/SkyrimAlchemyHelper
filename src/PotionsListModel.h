#ifndef POTIONSLISTMODEL_H
#define POTIONSLISTMODEL_H

#include <QAbstractTableModel>

class PotionsList;

class PotionsListModel : public QAbstractTableModel
{
public:
	PotionsListModel(QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

protected:
	PotionsList& m_potionsList;
};

#endif // POTIONSLISTMODEL_H
