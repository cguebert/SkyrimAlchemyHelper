#include <QtWidgets>

#include "PotionsListWidget.h"

#include "EffectsList.h"
#include "IngredientsList.h"
#include "PotionsList.h"

PotionsListWidget::PotionsListWidget(QWidget* parent)
	: QFrame(parent)
{
	setFrameShape(QFrame::StyledPanel);

	// TODO: what does it do on dark themes ?
	m_positiveColor.setRgb(0, 100, 0);
	m_negativeColor.setRgb(100, 0, 0);

	refreshList();
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
		const auto& potion = potions[potionsId[i]];
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

		auto infoLayout = new QVBoxLayout;
		auto goldLabel = new QLabel(QString("%1 gold").arg(static_cast<int>(potion.goldCost)));
		infoLayout->addWidget(goldLabel);
		potionLayout->addLayout(infoLayout);

		vLayout->addWidget(potionWidget);
	}

	vLayout->addStretch();

	setLayout(vLayout);
}