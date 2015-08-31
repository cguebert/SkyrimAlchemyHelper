#ifndef EFFECTSLISTWIDGET_H
#define EFFECTSLISTWIDGET_H

#include <QWidget>

class EffectsListModel;
class QPushButton;
class QTableView;

class EffectsListWidget : public QWidget
{
	Q_OBJECT
public:
	explicit EffectsListWidget(QWidget* parent = nullptr);

public slots:
	void beginReset();
	void endReset();

protected:
	QTableView *m_view;
	EffectsListModel *m_model;
};

#endif // EFFECTSLISTWIDGET_H
