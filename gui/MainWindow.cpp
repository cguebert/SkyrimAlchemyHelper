#include <QtWidgets>

#include "MainWindow.h"
#include "ConfigDialog.h"
#include "SaveDialog.h"

#include "EffectsSelector.h"
#include "FiltersWidget.h"
#include "IngredientsSelector.h"
#include "PotionsListWidget.h"
#include "PotionsList.h"
#include "Settings.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setWindowTitle("Skyrim Alchemy Helper");

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	setTabPosition(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea, QTabWidget::North);

	auto filtersScrollArea = new QScrollArea(this);
	filtersScrollArea->setWidgetResizable(true);
	m_filtersWidget = new FiltersWidget(this);
	filtersScrollArea->setWidget(m_filtersWidget);
	auto filtersDock = new QDockWidget(tr("Filters"));
	filtersDock->setObjectName("FiltersDock");
	filtersDock->setWidget(filtersScrollArea);
	filtersDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	addDockWidget(Qt::TopDockWidgetArea, filtersDock);

	auto ingredientsScrollArea = new QScrollArea(this);
	ingredientsScrollArea->setWidgetResizable(true);
	m_ingredientsSelector = new IngredientsSelector(this);
	ingredientsScrollArea->setWidget(m_ingredientsSelector);
	auto ingredientsDock = new QDockWidget(tr("Ingredients"));
	ingredientsDock->setObjectName("IngredientsDock");
	ingredientsDock->setWidget(ingredientsScrollArea);
	ingredientsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	ingredientsDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	addDockWidget(Qt::LeftDockWidgetArea, ingredientsDock);

	auto effectsScrollArea = new QScrollArea(this);
	effectsScrollArea->setWidgetResizable(true);
	m_effectsSelector = new EffectsSelector(this);
	effectsScrollArea->setWidget(m_effectsSelector);
	auto effectsDock = new QDockWidget(tr("Effects"));
	effectsDock->setObjectName("EffectsDock");
	effectsDock->setWidget(effectsScrollArea);
	effectsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	effectsDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	addDockWidget(Qt::LeftDockWidgetArea, effectsDock);
	tabifyDockWidget(effectsDock, ingredientsDock);

	QWidget* mainWidget = new QWidget(this);
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(0, 0, 0, 0);

	auto potionsScrollArea = new QScrollArea(this);
	m_potionsWidget = new PotionsListWidget;
	potionsScrollArea->setWidget(m_potionsWidget);
	potionsScrollArea->setFrameStyle(QFrame::NoFrame);
	potionsScrollArea->setWidgetResizable(true);
	vLayout->addWidget(potionsScrollArea);

	QPushButton* okButton = new QPushButton(tr("Exit"), this);
	connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
	QPushButton* configButton = new QPushButton(tr("Configuration"), this);
	connect(configButton, SIGNAL(clicked()), this, SLOT(editConfig()));
	QPushButton* saveButton = new QPushButton(tr("Game save"), this);
	connect(saveButton, SIGNAL(clicked()), this, SLOT(gameSaveInformation()));
	QPushButton* discoverButton = new QPushButton(tr("Discover effects"), this);
	connect(discoverButton, SIGNAL(clicked()), this, SLOT(discoverEffects()));
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->setContentsMargins(5, 5, 5, 5);
	buttonsLayout->addWidget(configButton);
	buttonsLayout->addWidget(saveButton);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(discoverButton);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(okButton);
	vLayout->addLayout(buttonsLayout);

	mainWidget->setLayout(vLayout);
	setCentralWidget(mainWidget);

	connect(this, SIGNAL(mainWindowShown()), this, SLOT(afterLaunch()), Qt::QueuedConnection);

	connect(m_ingredientsSelector, SIGNAL(ingredientFilterAction(FilterActionType, int)), m_filtersWidget, SLOT(ingredientFilterAction(FilterActionType, int)));
	connect(m_effectsSelector, SIGNAL(effectFilterAction(FilterActionType, int)), m_filtersWidget, SLOT(effectFilterAction(FilterActionType, int)));
	connect(m_filtersWidget, SIGNAL(modified()), m_potionsWidget, SLOT(refreshList()));

	readSettings();
}

QSize MainWindow::sizeHint() const
{
	return QSize(950, 800);
}

void MainWindow::readSettings()
{
	QSettings settings("SAH.ini", QSettings::IniFormat);
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());

	Settings::instance().load();
}

void MainWindow::writeSettings()
{
	QSettings settings("SAH.ini", QSettings::IniFormat);
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());

	Settings::instance().save();
}

void MainWindow::editConfig()
{
	ConfigDialog dlg(this);
	if (dlg.exec() && dlg.modified())
	{
		m_effectsSelector->updateList();
		m_ingredientsSelector->updateList();
		GameSave::instance().loadSaveFromConfig();
		m_filtersWidget->clear();
		PotionsList::instance().recomputeList();
		m_potionsWidget->refreshList();
	}
}

void MainWindow::afterLaunch()
{
	if (Settings::instance().isEmpty())
	{
		QMessageBox::information(this, tr("First launch"), tr("Please verify the configuration"));
		ConfigDialog dlg(this, true);
		if(dlg.exec())
			GameSave::instance().loadSaveFromConfig();
	}
}

void MainWindow::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);

	emit mainWindowShown();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	writeSettings();
	event->accept();
}

void MainWindow::gameSaveInformation()
{
	SaveDialog dlg(this);
	if (dlg.exec() && dlg.modified())
	{
		m_filtersWidget->clear();
		PotionsList::instance().recomputeList();
		m_potionsWidget->refreshList();
	}
}

void MainWindow::discoverEffects()
{
	m_filtersWidget->clear();
	PotionsList::instance().discoverEffects();
	m_potionsWidget->refreshList();
}
