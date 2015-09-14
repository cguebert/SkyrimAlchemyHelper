#include "EffectsSelector.h"
#include "Config.h"

EffectsSelector::EffectsSelector(QWidget* parent)
	: SelectorWidget(parent)
{
	updateList();
}

void EffectsSelector::updateList()
{
	const auto& config = Config::main();

	QStringList names, tooltips;
	for (const auto& effect : config.effects)
	{
		names << effect.name;
		tooltips << effect.tooltip;
	}
	setItems(names, tooltips);
}

bool EffectsSelector::filterAction(FilterActionType action, int id)
{
	emit effectFilterAction(action, id);
	return true;
}
