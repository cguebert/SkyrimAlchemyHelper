#pragma once

#include "SelectorWidget.h"

class IngredientsSelector : public SelectorWidget
{
	Q_OBJECT
public:
	explicit IngredientsSelector(QWidget* parent = nullptr);

public slots:
	void updateList();

protected:
	bool filterAction(FilterActionType action, int id) override;
};

