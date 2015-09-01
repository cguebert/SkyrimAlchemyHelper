#include <QtWidgets>

#include "MainWindow.h"
#include "ConfigDialog.h"

#include "PotionsListWidget.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setWindowTitle("Skyrim Alchemy Helper");

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
}

QSize MainWindow::sizeHint() const
{
	return QSize(800, 600);
}

void MainWindow::editConfig()
{
	ConfigDialog dlg(this);
	dlg.exec();
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