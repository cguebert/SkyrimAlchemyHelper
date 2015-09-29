/******************************************************************************
*                            Skyrim Alchemy Helper                            *
*******************************************************************************
*                                                                             *
* Copyright (C) 2015 Christophe Guebert                                       *
*                                                                             *
* This program is free software; you can redistribute it and/or modify        *
* it under the terms of the GNU General Public License as published by        *
* the Free Software Foundation; either version 2 of the License, or           *
* (at your option) any later version.                                         *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc.,     *
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                 *
*******************************************************************************
*                        Skyrim Alchemy Helper :: Gui                         *
*                                                                             *
*                  Contact: christophe.guebert+SAH@gmail.com                  *
******************************************************************************/
#include <QtWidgets>

#include "MainWindow.h"
#include "ConfigDialog.h"
#include "SaveDialog.h"

#include "FiltersWidget.h"
#include "PotionsListWidget.h"
#include "PotionsList.h"
#include "SelectorWidget.h"
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

	auto potionsScrollArea = new QScrollArea(this);
	m_potionsWidget = new PotionsListWidget;
	potionsScrollArea->setWidget(m_potionsWidget);
	potionsScrollArea->setFrameStyle(QFrame::NoFrame);
	potionsScrollArea->setWidgetResizable(true);
	setCentralWidget(potionsScrollArea);

	auto fileMenu = menuBar()->addMenu(tr("&File"));
	auto exitAction = new QAction(tr("E&xit"), this);
	exitAction->setShortcut(tr("Ctrl+Q"));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
	fileMenu->addAction(exitAction);

	auto editMenu = menuBar()->addMenu(tr("&Edit"));
	auto configAction = new QAction(tr("Con&figuration"), this);
	configAction->setShortcut(tr("Ctrl+F"));
	connect(configAction, SIGNAL(triggered()), this, SLOT(editConfig()));
	editMenu->addAction(configAction);

	auto gameSaveAction = new QAction(tr("&Game save"), this);
	gameSaveAction->setShortcut(tr("Ctrl+G"));
	connect(gameSaveAction, SIGNAL(triggered()), this, SLOT(gameSaveInformation()));
	editMenu->addAction(gameSaveAction);

	auto toolsMenu = menuBar()->addMenu(tr("&Tools"));
	auto discoverEffectsAction = new QAction(tr("&Discover effects"), this);
	discoverEffectsAction->setShortcut(tr("Ctrl+D"));
	connect(discoverEffectsAction, SIGNAL(triggered()), this, SLOT(discoverEffects()));
	toolsMenu->addAction(discoverEffectsAction);

	auto helpMenu = menuBar()->addMenu(tr("&Help"));
	auto aboutAction = new QAction(tr("&About"), this);
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
	helpMenu->addAction(aboutAction);

	connect(this, SIGNAL(mainWindowShown()), this, SLOT(afterLaunch()), Qt::QueuedConnection);

	connect(m_ingredientsSelector, SIGNAL(ingredientFilterAction(FilterActionType, int)), m_filtersWidget, SLOT(ingredientFilterAction(FilterActionType, int)));
	connect(m_effectsSelector, SIGNAL(effectFilterAction(FilterActionType, int)), m_filtersWidget, SLOT(effectFilterAction(FilterActionType, int)));
	connect(m_filtersWidget, SIGNAL(modified()), m_potionsWidget, SLOT(refreshList()));
	connect(m_filtersWidget, SIGNAL(ingredientFilterRemoved(int)), m_ingredientsSelector, SLOT(filterRemoved(int)));
	connect(m_filtersWidget, SIGNAL(effectFilterRemoved(int)), m_effectsSelector, SLOT(filterRemoved(int)));

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
		QMessageBox::information(this, tr("First launch"), tr("Please verify the configuration, then click \"Parse mods\"."));
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

void MainWindow::about()
{
	QMessageBox::about(this, tr("About"),
		tr("<h2>Skyrim Alchemy Helper 1.0</h2>"
		"<p>Copyright &copy; 2015 Christophe Guébert"
		"<p>Licence: GPL v3."));
}
