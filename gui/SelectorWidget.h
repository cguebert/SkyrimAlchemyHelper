#pragma once

#include <QWidget>

#include <vector>
#include <cstdint>

#include "FiltersWidget.h"

class QPushButton;
class QWidget;

class SelectorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SelectorWidget(QWidget* parent = nullptr);

	void setItems(QStringList items, QStringList tooltips);

public slots:
	void modifyFilter();
	void filterRemoved(int);

protected:
	virtual bool filterAction(FilterActionType actionType, int id) = 0;

	QStringList m_items, m_tooltips;

	struct ItemBox
	{
		QWidget *widget;
		QPushButton *addContainsButton, *addDoesNotContainButton, *removeButton;
	};

	using ItemBoxes = std::vector<ItemBox>;
	ItemBoxes m_itemBoxes;
};

