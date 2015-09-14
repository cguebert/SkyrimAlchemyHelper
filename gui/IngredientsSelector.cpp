#include "IngredientsSelector.h"
#include "Config.h"

IngredientsSelector::IngredientsSelector(QWidget* parent)
	: SelectorWidget(parent)
{
	updateList();
}

void IngredientsSelector::updateList()
{
	const auto& config = Config::main();

	QStringList names, tooltips;
	for (const auto& ingredient : config.ingredients)
	{
		names << ingredient.name;
		tooltips << ingredient.tooltip;
	}
	setItems(names, tooltips);
}

bool IngredientsSelector::filterAction(FilterActionType action, int id)
{
	emit ingredientFilterAction(action, id);
	return false;
}
