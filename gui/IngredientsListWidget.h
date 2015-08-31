#ifndef INGREDIENTSLISTWIDGET_H
#define INGREDIENTSLISTWIDGET_H

#include <QWidget>

class IngredientsListModel;
class QPushButton;
class QTableView;

class IngredientsListWidget : public QWidget
{
	Q_OBJECT
public:
	explicit IngredientsListWidget(QWidget* parent = nullptr);

public slots:
	void beginReset();
	void endReset();

protected:
	QTableView *m_view;
	IngredientsListModel *m_model;
};

#endif // INGREDIENTSLISTWIDGET_H
