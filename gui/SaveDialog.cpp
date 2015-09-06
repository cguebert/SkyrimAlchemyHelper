#include <QtWidgets>

#include "SaveDialog.h"

#include "GameSave.h"

SaveDialog::SaveDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Skyrim Alchemy Helper - Save information");

	auto vLayout = new QVBoxLayout;

	auto& gamesave = GameSave::instance();
	auto screenshotLabel = new QLabel; 
	screenshotLabel->setPixmap(gamesave.screenshot());
	vLayout->addWidget(screenshotLabel);

	vLayout->addStretch();

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

}

QSize SaveDialog::sizeHint() const
{
	return QSize(700, 500);
}
