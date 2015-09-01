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
	bool setFilterContains(int32_t id) override;
	bool setFilterDoesNotContain(int32_t id) override;
	bool removeFilter(int32_t id) override;
};

