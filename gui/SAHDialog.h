#ifndef SAHDIALOG_H
#define SAHDIALOG_H

#include <QDialog>

class QTabWidget;

class SAHDialog : public QDialog
{
	Q_OBJECT

public:
	SAHDialog(QWidget *parent = 0);

	QSize sizeHint() const;

public slots:
	void saveConfig();

protected:
	QTabWidget* m_tabWidget;
};

#endif // SAHDIALOG_H
