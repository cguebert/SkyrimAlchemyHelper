#ifndef PLUGINSLISTWIDGET_H
#define PLUGINSLISTWIDGET_H

#include <QWidget>

class PluginsListModel;
class QPushButton;
class QTableView;

class PluginsListWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PluginsListWidget(QWidget* parent = nullptr);

public slots:
	void beginReset();
	void endReset();

protected:
	QTableView *m_view;
	PluginsListModel *m_model;
};

#endif // PLUGINSLISTWIDGET_H
