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

		QString tooltip;
		for (const auto& ingId : effect.ingredients)
			tooltip += ingredients[ingId].name + "\n";
		tooltips << tooltip.trimmed();
	}
	setItems(names, tooltips);
}

bool EffectsSelector::filterAction(FilterActionType action, int id)
{
	emit effectFilterAction(action, id);
	return true;
}
