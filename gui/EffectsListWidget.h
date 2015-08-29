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

#endif // EFFECTSLISTWIDGET_H
