#ifndef POTIONSLISTWIDGET_H
#define POTIONSLISTWIDGET_H

#include <QFrame>

class PotionsListWidget : public QFrame
{
	Q_OBJECT
public:
	explicit PotionsListWidget(QWidget* parent = nullptr);

public slots:
	void refreshList();

protected:
	QColor m_positiveColor, m_negativeColor;
};

#endif // POTIONSLISTWIDGET_H
