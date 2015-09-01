#include "EffectsSelector.h"
#include "EffectsList.h"

EffectsSelector::EffectsSelector(QWidget* parent)
	: SelectorWidget(parent)
{
	updateList();
}

void EffectsSelector::updateList()
{
	const auto& effects = EffectsList::instance().effects();

	QStringList list;
	for (const auto& effect : effects)
		list << effect.name;
	setItems(list);
}

bool EffectsSelector::filterAction(FilterActionType action, int id)
{
	emit effectFilterAction(action, id);
	return true;
}
