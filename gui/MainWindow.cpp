#include <QtWidgets>

#include "MainWindow.h"
#include "ConfigDialog.h"

#include "EffectsSelector.h"
#include "IngredientsSelector.h"
#include "PotionsListWidget.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setWindowTitle("Skyrim Alchemy Helper");

	auto ingredientsScrollArea = new QScrollArea(this);
	ingredientsScrollArea->setFrameStyle(QFrame::NoFrame);
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
	effectsScrollArea->setFrameStyle(QFrame::NoFrame);
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

	setTabPosition(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea, QTabWidget::North);

	QWidget* mainWidget = new QWidget(this);
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto potionsWidget = new PotionsListWidget;
	vLayout->addWidget(potionsWidget);

	QPushButton* okButton = new QPushButton(tr("Exit"), this);
	connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
	QPushButton* configButton = new QPushButton(tr("Config"), this);
	connect(configButton, SIGNAL(clicked()), this, SLOT(editConfig()));
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(configButton);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(okButton);
	vLayout->addLayout(buttonsLayout);

	mainWidget->setLayout(vLayout);
	setCentralWidget(mainWidget);

	connect(this, SIGNAL(mainWindowShown()), this, SLOT(afterLaunch()), Qt::QueuedConnection);

	readSettings();
}

QSize MainWindow::sizeHint() const
{
	return QSize(800, 600);
}

void MainWindow::readSettings()
{
	QSettings settings;
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
}

void MainWindow::editConfig()
{
	ConfigDialog dlg(this);
	dlg.exec();

	m_effectsSelector->updateList();
	m_ingredientsSelector->updateList();
}

void MainWindow::afterLaunch()
{
	QSettings settings;
	if (!settings.contains("useModOrganizer"))
	{
		QMessageBox::information(this, tr("First launch"), tr("Please verify the configuration"));
		ConfigDialog dlg(this, true);
		dlg.exec();
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