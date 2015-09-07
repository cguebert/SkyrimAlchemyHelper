#pragma once

#include <QDialog>

class QWidget;

class SaveDialog : public QDialog
{
	Q_OBJECT

public:
	SaveDialog(QWidget *parent = 0);

	QSize sizeHint() const;

public slots:
	void refreshInformation();

protected:
	QWidget* m_saveInfoContainer;
};
