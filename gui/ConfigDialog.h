#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

class QTabWidget;

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog(QWidget *parent = 0);

	QSize sizeHint() const;

public slots:
	void saveConfig();
	void parseMods();

protected:
	QTabWidget* m_tabWidget;
};

#endif // CONFIGDIALOG_H
