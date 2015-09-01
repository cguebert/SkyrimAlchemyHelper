#pragma once

#include "SelectorWidget.h"

class EffectsSelector : public SelectorWidget
{
	Q_OBJECT
public:
	explicit EffectsSelector(QWidget* parent = nullptr);

public slots:
	void updateList();

protected:
	bool setFilterContains(int32_t id) override;
	bool setFilterDoesNotContain(int32_t id) override;
	bool removeFilter(int32_t id) override;
};

