#include "EffectsSelector.h"
#include "EffectsList.h"
#include "IngredientsList.h"

EffectsSelector::EffectsSelector(QWidget* parent)
	: SelectorWidget(parent)
{
	updateList();
}

void EffectsSelector::updateList()
{
	const auto& effects = EffectsList::instance().effects();
	const auto& ingredients = IngredientsList::instance().ingredients();

	QStringList names, tooltips;
	for (const auto& effect : effects)
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
