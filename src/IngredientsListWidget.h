#ifndef INGREDIENTSLISTWIDGET_H
#define INGREDIENTSLISTWIDGET_H

#include <QWidget>

class QPushButton;
class QSortFilterProxyModel;
class QTableView;

class IngredientsListWidget : public QWidget
{
	Q_OBJECT
public:
	explicit IngredientsListWidget(QWidget* parent = nullptr);

signals:

public slots:
	void currentChanged(const QModelIndex&, const QModelIndex&);
	void addRow();
	void removeRow();

protected:
	QTableView *m_view;
	QSortFilterProxyModel *m_model;
	QPushButton *m_addButton, *m_removeButton;
};

#endif // INGREDIENTSLISTWIDGET_H
