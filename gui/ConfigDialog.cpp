#include <QtWidgets>

#include "ConfigDialog.h"
#include "ConfigPane.h"
#include "EffectsListWidget.h"
#include "IngredientsListWidget.h"
#include "PluginsListWidget.h"

ConfigDialog::ConfigDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Skyrim Alchemy Helper - Config");
	QVBoxLayout* vLayout = new QVBoxLayout;

	m_tabWidget = new QTabWidget;
	vLayout->addWidget(m_tabWidget);

	auto configWidget = new ConfigPane;
	m_tabWidget->addTab(configWidget, "General");

	auto pluginsWidget = new PluginsListWidget;
	m_tabWidget->addTab(pluginsWidget, "Plugins");

	auto effectsWidget = new EffectsListWidget;
	m_tabWidget->addTab(effectsWidget, "Effects");

	auto ingredientsWidget = new IngredientsListWidget;
	m_tabWidget->addTab(ingredientsWidget, "Ingredients");

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

QSize ConfigDialog::sizeHint() const
{
	return QSize(800, 600);
}

void ConfigDialog::saveConfig()
{

}

void ConfigDialog::parseMods()
{

}

