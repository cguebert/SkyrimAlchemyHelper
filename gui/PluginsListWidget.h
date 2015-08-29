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

protected:
	QTableView *m_view;
	QSortFilterProxyModel *m_model;
};

#endif // PLUGINSLISTWIDGET_H
