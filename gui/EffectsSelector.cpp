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

	ItemList list;
	for (const auto& effect : effects)
		list.emplace_back(effect.id, effect.name);
	setItems(std::move(list));
}

bool EffectsSelector::setFilterContains(int32_t id)
{
	return false;
}

bool EffectsSelector::setFilterDoesNotContain(int32_t id)
{
	return false;
}

bool EffectsSelector::removeFilter(int32_t id)
{
	return false;
}
