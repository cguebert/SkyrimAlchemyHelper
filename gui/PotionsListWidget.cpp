#include <QtWidgets>

#include "PotionsListWidget.h"

#include "EffectsList.h"
#include "IngredientsList.h"
#include "PotionsList.h"
#include "GameSave.h"

PotionsListWidget::PotionsListWidget(QWidget* parent)
	: QFrame(parent)
{
	setFrameShape(QFrame::StyledPanel);

	// TODO: what does it do on dark themes ?
	m_positiveColor.setRgb(0, 100, 0);
	m_negativeColor.setRgb(100, 0, 0);

	refreshList();
}

void PotionsListWidget::nbCraftable(int id, QString& text, QString& tooltip)
{
	const auto& potionsList = PotionsList::instance();
	const auto& ingredients = IngredientsList::instance().ingredients();
	const auto& potions = potionsList.allPotions();
	const auto& potionsId = potionsList.sortedPotions();
	const auto& ingredientsCount = potionsList.additionalData()[id].ingredientsCount;

	const auto& potion = potions[potionsId[id]];
	int ing0 = potion.ingredients[0], ing1 = potion.ingredients[1], ing2 = potion.ingredients[2];
	int nb0 = ingredientsCount[0], nb1 = ingredientsCount[1];
	int minNb = std::min(nb0, nb1);
	tooltip = QString("%1 - %2\n%3 - %4").arg(ingredients[ing0].name).arg(nb0)
		.arg(ingredients[ing1].name).arg(nb1);
	if (ing2 != -1)
	{
		int nb2 = ingredientsCount[2];
		minNb = std::min(minNb, nb2);
		tooltip += QString("\n%1 - %2").arg(ingredients[ing2].name).arg(nb2);
	}

	if (!minNb)				text = tr("Cannot craft this potion");
	else if (minNb == 1)	text = tr("Can craft 1 potion");
	else					text = tr("Can craft %1 potions").arg(minNb);
}

void PotionsListWidget::nbDiscoveredEffects(int id, QString& text, QString& tooltip)
{
	const auto& ingredients = IngredientsList::instance().ingredients();
	const auto& effects = EffectsList::instance().effects();
	const auto& discoveredEffects = PotionsList::instance().additionalData()[id].discoveredEffects;

	tooltip = "";
	for (auto discovered : discoveredEffects)
		tooltip += QString("%1 - %2\n").arg(ingredients[discovered.first].name).arg(effects[discovered.second].name);
	tooltip = tooltip.trimmed();

	const auto nbDiscovered = discoveredEffects.size();
	if (!nbDiscovered)			text = tr("All effects known");
	else if (nbDiscovered == 1)	text = tr("1 undiscovered effect");
	else						text = tr("%1 undiscovered effects").arg(nbDiscovered);
}

void PotionsListWidget::refreshList()
{
	auto l = layout();
	if (l)
		QWidget().setLayout(l);

	const auto& ingredients = IngredientsList::instance().ingredients();
	const auto& effects = EffectsList::instance().effects();
	const auto& potions = PotionsList::instance().allPotions();
	const auto& potionsId = PotionsList::instance().sortedPotions();
	const auto& ingredientsCount = GameSave::instance().ingredientsCount();
	const auto& knownIngredients = GameSave::instance().knownIngredients();
	int nbPotions = potionsId.size();
	const int showNbPotions = 50;

	QVBoxLayout* vLayout = new QVBoxLayout;
	QString potionsCountText;
	if (!nbPotions)
		potionsCountText = tr("No potions corresponding to these filters");
	else if (nbPotions > showNbPotions)
		potionsCountText = tr("Showing %1 potions out of %2").arg(showNbPotions).arg(nbPotions);
	else
		potionsCountText = tr("Showing all %1 potions").arg(nbPotions);
	auto label = new QLabel(potionsCountText);
	vLayout->addWidget(label);

	for (int i = 0, nb = std::min(showNbPotions, nbPotions); i < nb; ++i)
	{
		const auto potionId = potionsId[i];
		const auto& potion = potions[potionId];
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

		potionLayout->addLayout(ingredientsLayout);

		auto effectsLayout = new QVBoxLayout;
		auto strengthLayout = new QVBoxLayout;
		for (int effId = 0; effId < PotionsList::maxEffectsPerPotion
			&& potion.effects[effId] != -1; ++effId)
		{
			const auto& effect = effects[potion.effects[effId]];
			auto effLabel = new QLabel(effect.name);
			effLabel->setAutoFillBackground(true);
			QPalette pal = effLabel->palette();
			if (effect.flags & EffectFlags::Hostile)
				pal.setColor(QPalette::WindowText, m_negativeColor);
			else
				pal.setColor(QPalette::WindowText, m_positiveColor);
			effLabel->setPalette(pal);

			effLabel->setToolTip(effect.tooltip);
			effectsLayout->addWidget(effLabel);

			QString strengthText, magText, durText;
			magText = QString::number(potion.magnitudes[effId], 'f', 0);
			durText = QString::number(potion.durations[effId], 'f', 0);
			if (potion.durations[effId] > 0)
			{
				if (potion.magnitudes[effId] > 0)
					strengthText = QString("%1 during %2 sec").arg(magText).arg(durText);
				else
					strengthText = QString("during %1 sec").arg(durText);
			}
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

		potionLayout->addLayout(effectsLayout);
		potionLayout->addLayout(strengthLayout);

		// Last column
		auto infoLayout = new QVBoxLayout;
		auto goldLabel = new QLabel(QString("%1 gold").arg(static_cast<int>(potion.goldCost)));
		infoLayout->addWidget(goldLabel);

		if (GameSave::instance().isLoaded())
		{
			QString text, tooltip;
			nbCraftable(i, text, tooltip);
			auto nbCraftableLabel = new QLabel(text);
			nbCraftableLabel->setToolTip(tooltip);
			infoLayout->addWidget(nbCraftableLabel);

			nbDiscoveredEffects(i, text, tooltip);	
			auto discoveredLabel = new QLabel(text);
			discoveredLabel->setToolTip(tooltip);
			infoLayout->addWidget(discoveredLabel);
		}

		potionLayout->addLayout(infoLayout);

		vLayout->addWidget(potionWidget);
	}

	vLayout->addStretch();

	setLayout(vLayout);
}