#include <QtWidgets>

#include "SAHDialog.h"
#include "ConfigDialog.h"

#include "PotionsListWidget.h"

SAHDialog::SAHDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Skyrim Alchemy Helper");
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto potionsWidget = new PotionsListWidget;
	vLayout->addWidget(potionsWidget);

	QPushButton* okButton = new QPushButton(tr("Exit"), this);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QPushButton* configButton = new QPushButton(tr("Config"), this);
	connect(configButton, SIGNAL(clicked()), this, SLOT(editConfig()));
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(configButton);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(okButton);
	vLayout->addLayout(buttonsLayout);

	setLayout(vLayout);

	connect(this, SIGNAL(dialogShown()), this, SLOT(afterLaunch()), Qt::QueuedConnection);
}

QSize SAHDialog::sizeHint() const
{
	return QSize(800, 600);
}

void SAHDialog::editConfig()
{
	ConfigDialog dlg(this);
	dlg.exec();
}

void SAHDialog::afterLaunch()
{
	QSettings settings;
	if (!settings.contains("useModOrganizer"))
	{
		QMessageBox::information(this, tr("First launch"), tr("Please verify the configuration"));
		ConfigDialog dlg(this, true);
		dlg.exec();
	}
}

void SAHDialog::showEvent(QShowEvent* event) 
{
	QDialog::showEvent(event);

	emit dialogShown();
}