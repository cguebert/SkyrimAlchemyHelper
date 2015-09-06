#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

class ConfigPane;
class QTabWidget;

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog(QWidget *parent = 0, bool firstLaunch = false);

	QSize sizeHint() const;
	bool areListsModified() const;

public slots:
	void onOk();

protected:
	QTabWidget* m_tabWidget;
	ConfigPane* m_configPane;
};

#endif // CONFIGDIALOG_H
