#pragma once

#include <QMainWindow>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);

	QSize sizeHint() const;

signals:
	void mainWindowShown();

public slots:
	void editConfig();
	void afterLaunch();

protected:
	void showEvent(QShowEvent* event);
	
};
