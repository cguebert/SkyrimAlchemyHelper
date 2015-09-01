#pragma once

#include "SelectorWidget.h"

class IngredientsSelector : public SelectorWidget
{
	Q_OBJECT
public:
	explicit IngredientsSelector(QWidget* parent = nullptr);

signals:
	void ingredientFilterAction(FilterActionType action, int id);

public slots:
	void updateList();

protected:
	bool filterAction(FilterActionType action, int id) override;
};

