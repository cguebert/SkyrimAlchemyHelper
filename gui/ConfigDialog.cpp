#include <QtWidgets>

#include "ConfigDialog.h"
#include "ConfigPane.h"
#include "EffectsListWidget.h"
#include "IngredientsListWidget.h"
#include "PluginsListWidget.h"

ConfigDialog::ConfigDialog(QWidget *parent, bool firstLaunch)
	: QDialog(parent)
{
	setWindowTitle("Skyrim Alchemy Helper - Config");
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(5, 5, 5, 5);

	auto tabWidget = new QTabWidget;
	vLayout->addWidget(tabWidget);

	m_configPane = new ConfigPane(this, firstLaunch);
	tabWidget->addTab(m_configPane, "General");

	auto pluginsWidget = new PluginsListWidget;
	tabWidget->addTab(pluginsWidget, "Plugins");

	auto effectsWidget = new EffectsListWidget;
	tabWidget->addTab(effectsWidget, "Effects");

	auto ingredientsWidget = new IngredientsListWidget;
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

bool ConfigDialog::areListsModified() const
{
	return m_configPane->areListsModified();
}

void ConfigDialog::onOk()
{
	if (m_configPane->testConfig())
	{
		m_configPane->saveConfig();
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
			accept();
		}
		else if (button == QMessageBox::No)
			reject();
	}
}
