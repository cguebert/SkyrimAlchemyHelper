#pragma once

#include <QDialog>

class SaveDialog : public QDialog
{
	Q_OBJECT

public:
	SaveDialog(QWidget *parent = 0);

	QSize sizeHint() const;

public slots:

protected:
};
