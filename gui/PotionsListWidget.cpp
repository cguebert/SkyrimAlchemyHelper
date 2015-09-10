#include <QtWidgets>

#include "PotionsListWidget.h"

#include "EffectsList.h"
#include "IngredientsList.h"
#include "PotionsList.h"

PotionsListWidget::PotionsListWidget(QWidget* parent)
	: QFrame(parent)
{
	setFrameShape(QFrame::StyledPanel);
	PotionsList::instance().setUpdateCallback([this]() { refreshList(); });
	refreshList();
}

void PotionsListWidget::refreshList()
{
	auto l = layout();
	if (l)
		QWidget().setLayout(l);

	const auto& ingredients = IngredientsList::instance().ingredients();
	const auto& effects = EffectsList::instance().effects();
	const auto& potions = PotionsList::instance().filteredPotions();
	int nbPotions = potions.size();

	QVBoxLayout* vLayout = new QVBoxLayout;
	auto label = new QLabel("Number of potions : " + QString::number(potions.size()));
	vLayout->addWidget(label);

	for (int i = 0, nb = std::min(50, nbPotions); i < nb; ++i)
	{
		const auto& potion = potions[i];
		auto potionWidget = new QFrame;
		potionWidget->setFrameShape(QFrame::Box);
		auto potionLayout = new QHBoxLayout(potionWidget);

		auto ingredientsLayout = new QVBoxLayout;
		for (int ingId = 0; ingId < PotionsList::maxIngredientsPerPotion
			&& potion.ingredients[ingId] != -1; ++ingId)
		{
			const auto& ingredient = ingredients[potion.ingredients[ingId]];
			auto ingLabel = new QLabel(ingredient.name);
			ingLabel->setToolTip(ingredient.tooltip);
			ingredientsLayout->addWidget(ingLabel);
		}
	//	ingredientsLayout->addStretch();
		potionLayout->addLayout(ingredientsLayout);

		auto effectsLayout = new QVBoxLayout;
		auto strengthLayout = new QVBoxLayout;
		for (int effId = 0; effId < PotionsList::maxEffectsPerPotion
			&& potion.effects[effId] != -1; ++effId)
		{
			const auto& effect = effects[potion.effects[effId]];
			auto effLabel = new QLabel(effect.name);
			effLabel->setToolTip(effect.tooltip);
			effectsLayout->addWidget(effLabel);

			QString strengthText, magText, durText;
			magText = QString::number(potion.magnitudes[effId], 'f', 2);
			durText = QString::number(potion.durations[effId], 'f', 1);
			if (potion.durations[effId] > 0)
				strengthText = QString("%1 during %2 sec").arg(magText).arg(durText);
			else
				strengthText = QString::number(potion.magnitudes[effId], 'f', 2);
			auto strengthLabel = new QLabel(strengthText);

			// Using the description of the magical effect
			if (!effect.description.isEmpty())
			{
				QString tooltip = effect.description;
				tooltip.replace("<mag>", magText);
				tooltip.replace("<dur>", durText);
				strengthLabel->setToolTip(tooltip);
			}
			strengthLayout->addWidget(strengthLabel);
		}
	//	effectsLayout->addStretch();
		potionLayout->addLayout(effectsLayout);
		potionLayout->addLayout(strengthLayout);

		vLayout->addWidget(potionWidget);
	}

	vLayout->addStretch();

	setLayout(vLayout);
}