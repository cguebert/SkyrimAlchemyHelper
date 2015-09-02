#ifndef POTIONSLISTWIDGET_H
#define POTIONSLISTWIDGET_H

#include <QFrame>

class PotionsListWidget : public QFrame
{
	Q_OBJECT
public:
	explicit PotionsListWidget(QWidget* parent = nullptr);

signals:

public slots:
	void refreshList();

protected:

};

#endif // POTIONSLISTWIDGET_H
