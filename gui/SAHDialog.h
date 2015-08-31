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

signals:
	void dialogShown();

public slots:
	void editConfig();
	void afterLaunch();

protected:
	void showEvent(QShowEvent* event);

	QTabWidget* m_tabWidget;
};

#endif // SAHDIALOG_H
