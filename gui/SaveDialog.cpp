#include <QtWidgets>

#include "SaveDialog.h"

#include "GameSave.h"

SaveDialog::SaveDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Skyrim Alchemy Helper - Save information");

	auto vLayout = new QVBoxLayout;

	auto& gamesave = GameSave::instance();
	auto loaded = gamesave.isLoaded();
	auto screenshotLabel = new QLabel("Not loaded"); 
	if (loaded)
		screenshotLabel->setPixmap(gamesave.screenshot());
	vLayout->addWidget(screenshotLabel);

	if (loaded)
	{
		auto knownIng = gamesave.knownIngredients();
		int knownEffects = 0;
		for (auto ing : knownIng)
		{
			for (int i = 0; i < 4; ++i)
				if (ing.second[i])
					++knownEffects;
		}

		int nbIng = knownIng.size();
		auto ingLabel = new QLabel(QString("%1 known ingredients, with %2% of discovered effects").arg(nbIng).arg(nbIng ? knownEffects * 25 / nbIng : 0));
		vLayout->addWidget(ingLabel);
	}

	vLayout->addStretch();

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

}

QSize SaveDialog::sizeHint() const
{
	return QSize(700, 500);
}
