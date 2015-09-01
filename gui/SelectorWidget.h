#pragma once

#include <QWidget>

#include <vector>
#include <cstdint>

#include "FiltersWidget.h"

class QVBoxLayout;

class SelectorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SelectorWidget(QWidget* parent = nullptr);

	void setItems(QStringList items);

public slots:
	void modifyFilter();

protected:
	virtual bool filterAction(FilterActionType actionType, int id) = 0;

	QStringList m_items;
};

