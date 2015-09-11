#include <QtWidgets>

#include "ConfigDialog.h"
#include "ConfigPane.h"
#include "EffectsListWidget.h"
#include "IngredientsListWidget.h"
#include "PluginsListWidget.h"

ConfigDialog::ConfigDialog(QWidget *parent, bool firstLaunch)
	: QDialog(parent)
{
	// Copy main lists
	m_effectsList = EffectsList::instance();
	m_ingredientsList = IngredientsList::instance();
	m_pluginsList = PluginsList::instance();

	setWindowTitle("Skyrim Alchemy Helper - Config");
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(5, 5, 5, 5);

	auto tabWidget = new QTabWidget;
	vLayout->addWidget(tabWidget);

	m_configPane = new ConfigPane(m_ingredientsList, m_effectsList, m_pluginsList, firstLaunch);
	tabWidget->addTab(m_configPane, "General");

	auto pluginsWidget = new PluginsListWidget(m_pluginsList);
	tabWidget->addTab(pluginsWidget, "Plugins");

	auto effectsWidget = new EffectsListWidget(m_effectsList);
	tabWidget->addTab(effectsWidget, "Effects");

	auto ingredientsWidget = new IngredientsListWidget(m_ingredientsList, m_effectsList, m_pluginsList);
	tabWidget->addTab(ingredientsWidget, "Ingredients");

	QPushButton* okButton = new QPushButton(tr("Ok"), this);
	connect(okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(okButton);
	buttonsLayout->addWidget(cancelButton);
	vLayout->addLayout(buttonsLayout);

	setLayout(vLayout);

	connect(m_configPane, SIGNAL(startModsParse()), pluginsWidget, SLOT(beginReset()));
	connect(m_configPane, SIGNAL(startModsParse()), effectsWidget, SLOT(beginReset()));
	connect(m_configPane, SIGNAL(startModsParse()), ingredientsWidget, SLOT(beginReset()));

	connect(m_configPane, SIGNAL(endModsParse()), pluginsWidget, SLOT(endReset()));
	connect(m_configPane, SIGNAL(endModsParse()), effectsWidget, SLOT(endReset()));
	connect(m_configPane, SIGNAL(endModsParse()), ingredientsWidget, SLOT(endReset()));
}

QSize ConfigDialog::sizeHint() const
{
	return QSize(700, 500);
}

bool ConfigDialog::modified() const
{
	return m_configPane->modified();
}

void ConfigDialog::onOk()
{
	if (m_configPane->testConfig())
	{
		m_configPane->saveConfig();
		copyLists();
		accept();
	}
	else
	{
		auto button = QMessageBox::question(this, tr("Invalid configuration"),
			tr("The current configuration is not valid, save it anyway?"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if (button == QMessageBox::Yes)
		{
			m_configPane->saveConfig();
			copyLists();
			accept();
		}
		else if (button == QMessageBox::No)
			reject();
	}
}

void ConfigDialog::copyLists()
{
	EffectsList::instance() = m_effectsList;
	IngredientsList::instance() = m_ingredientsList;
	PluginsList::instance() = m_pluginsList;

	m_pluginsList.saveList();
	m_effectsList.saveList();
	m_ingredientsList.saveList();
}
