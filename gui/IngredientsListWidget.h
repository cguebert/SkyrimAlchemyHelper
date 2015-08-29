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

protected:
	QTableView *m_view;
	QSortFilterProxyModel *m_model;
};

#endif // INGREDIENTSLISTWIDGET_H
