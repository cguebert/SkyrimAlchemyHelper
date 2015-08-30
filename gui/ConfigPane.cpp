#include <QtWidgets>

#include "ConfigPane.h"

ConfigPane::ConfigPane(QWidget *parent)
	: QWidget(parent)
{
	auto gridLayout = new QGridLayout;

	auto dataLabel = new QLabel(tr("Skyrim Data folder"));
	m_dataPathEdit = new QLineEdit;
	auto dataButton = new QPushButton("...");
	connect(dataButton, SIGNAL(clicked()), this, SLOT(editDataPath()));
	gridLayout->addWidget(dataLabel, 0, 0);
	gridLayout->addWidget(m_dataPathEdit, 0, 1);
	gridLayout->addWidget(dataButton, 0, 2);

	auto pluginsListLabel = new QLabel(tr("Plugins list file"));
	m_pluginsListPathEdit = new QLineEdit;
	auto pluginListButton = new QPushButton("...");
	connect(pluginListButton, SIGNAL(clicked()), this, SLOT(editPluginsPath()));
	gridLayout->addWidget(pluginsListLabel, 1, 0);
	gridLayout->addWidget(m_pluginsListPathEdit, 1, 1);
	gridLayout->addWidget(pluginListButton, 1, 2);

	auto savesLabel = new QLabel(tr("Saves folder"));
	m_savesPathEdit = new QLineEdit;
	auto savesButton = new QPushButton("...");
	connect(savesButton, SIGNAL(clicked()), this, SLOT(editSavesPath()));
	gridLayout->addWidget(savesLabel, 2, 0);
	gridLayout->addWidget(m_savesPathEdit, 2, 1);
	gridLayout->addWidget(savesButton, 2, 2);

	auto useModOrganizerCheckbox = new QCheckBox("Use Mod Organizer");
	connect(useModOrganizerCheckbox, SIGNAL(stateChanged(int)), this, SLOT(useModOrganizerChanged(int)));
	gridLayout->addWidget(useModOrganizerCheckbox, 3, 0, 1, 3);

	auto modOrganizerLabel = new QLabel(tr("Mod Organizer"));
	m_modOrganizerPathEdit = new QLineEdit;
	m_modOrganizerPathButton = new QPushButton("...");
	connect(m_modOrganizerPathButton, SIGNAL(clicked()), this, SLOT(editModOrganizerPath()));
	m_modOrganizerPathEdit->setEnabled(false);
	m_modOrganizerPathButton->setEnabled(false);
	gridLayout->addWidget(modOrganizerLabel, 4, 0);
	gridLayout->addWidget(m_modOrganizerPathEdit, 4, 1);
	gridLayout->addWidget(m_modOrganizerPathButton, 4, 2);

	auto buttonsLayout = new QHBoxLayout;
	auto parseModsButton = new QPushButton(tr("Parse mods"));
	connect(parseModsButton, SIGNAL(clicked()), this, SLOT(parseMods()));
	buttonsLayout->addWidget(parseModsButton);
	buttonsLayout->addStretch();
	gridLayout->addLayout(buttonsLayout, 5, 0, 1, 3);

	auto stretchLayout = new QVBoxLayout;
	stretchLayout->addStretch();
	gridLayout->addLayout(stretchLayout, 6, 0);

	setLayout(gridLayout);
}

void ConfigPane::loadConfig()
{

}

void ConfigPane::saveConfig()
{

}

void ConfigPane::parseMods()
{

}

void ConfigPane::editDataPath()
{
	auto dir = QFileDialog::getExistingDirectory(this, tr("Skyrim Data Folder"), m_dataPathEdit->text());
	m_dataPathEdit->setText(dir);
}

void ConfigPane::editPluginsPath()
{
	auto dir = QFileDialog::getOpenFileName(this, tr("Plugins list path"), m_pluginsListPathEdit->text(), tr("Plugins File (plugins.txt)"));
	m_pluginsListPathEdit->setText(dir);
}

void ConfigPane::editSavesPath()
{
	auto dir = QFileDialog::getExistingDirectory(this, tr("Saves Folder"), m_savesPathEdit->text());
	m_savesPathEdit->setText(dir);
}

void ConfigPane::editModOrganizerPath()
{
	auto dir = QFileDialog::getOpenFileName(this, tr("Mod Organizer path"), m_modOrganizerPathEdit->text(), tr("Mod Organizer (ModOrganizer.exe)"));
	m_modOrganizerPathEdit->setText(dir);
}

void ConfigPane::useModOrganizerChanged(int state)
{
	m_modOrganizerPathEdit->setEnabled(state == Qt::Checked);
	m_modOrganizerPathButton->setEnabled(state == Qt::Checked);
}