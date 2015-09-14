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
	void nbCraftable(int id, QString& text, QString& tooltip);
	void nbDiscoveredEffects(int id, QString& text, QString& tooltip);

	QColor m_positiveColor, m_negativeColor;
};

#endif // POTIONSLISTWIDGET_H
