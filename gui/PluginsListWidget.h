#ifndef PLUGINSLISTWIDGET_H
#define PLUGINSLISTWIDGET_H

#include <QWidget>

class QPushButton;
class QSortFilterProxyModel;
class QTableView;

class PluginsListWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PluginsListWidget(QWidget* parent = nullptr);

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

#endif // PLUGINSLISTWIDGET_H
