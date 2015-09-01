#pragma once

#include "SelectorWidget.h"

class EffectsSelector : public SelectorWidget
{
	Q_OBJECT
public:
	explicit EffectsSelector(QWidget* parent = nullptr);

signals:
	void effectFilterAction(FilterActionType action, int id);

public slots:
	void updateList();

protected:
	bool filterAction(FilterActionType action, int id) override;
};

