#include <QtWidgets>

#include "SAHDialog.h"

#include "EffectsListWidget.h"
#include "IngredientsListWidget.h"
#include "PluginsListWidget.h"
#include "PotionsListWidget.h"

#include "EffectsList.h"
#include "IngredientsList.h"
#include "PluginsList.h"

SAHDialog::SAHDialog(QWidget *parent)
	: QDialog(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	m_tabWidget = new QTabWidget;
	vLayout->addWidget(m_tabWidget);

	auto pluginsWidget = new PluginsListWidget;
	m_tabWidget->addTab(pluginsWidget, "Plugins");

	auto effectsWidget = new EffectsListWidget;
	m_tabWidget->addTab(effectsWidget, "Effects");

	auto ingredientsWidget = new IngredientsListWidget;
	m_tabWidget->addTab(ingredientsWidget, "Ingredients");

	auto potionssWidget = new PotionsListWidget;
	m_tabWidget->addTab(potionssWidget, "Potions");

	QPushButton* okButton = new QPushButton(tr("Ok"), this);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(okButton);
	buttonsLayout->addWidget(cancelButton);
	vLayout->addLayout(buttonsLayout);

	setLayout(vLayout);

	connect(this, SIGNAL(accepted()), this, SLOT(saveConfig()));
}

QSize SAHDialog::sizeHint() const
{
	return QSize(800, 600);
}

void SAHDialog::saveConfig()
{
	EffectsList::GetInstance().saveList();
	IngredientsList::GetInstance().saveList();
	PluginsList::GetInstance().saveList();
}
