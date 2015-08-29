#ifndef EFFECTSLISTWIDGET_H
#define EFFECTSLISTWIDGET_H

#include <QWidget>

class QPushButton;
class QSortFilterProxyModel;
class QTableView;

class EffectsListWidget : public QWidget
{
	Q_OBJECT
public:
	explicit EffectsListWidget(QWidget* parent = nullptr);

protected:
	QTableView *m_view;
	QSortFilterProxyModel *m_model;
};

#endif // EFFECTSLISTWIDGET_H
