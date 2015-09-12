#include <QtWidgets>

#include "SaveDialog.h"
#include "InventoryWidget.h"
#include "KnownIngredientsWidget.h"

SaveDialog::SaveDialog(QWidget *parent)
	: QDialog(parent)
{
	m_gameSave = GameSave::instance();
	setWindowTitle("Skyrim Alchemy Helper - Save information");

	auto vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(5, 5, 5, 5);

	auto tabWidget = new QTabWidget;
	vLayout->addWidget(tabWidget);

	m_saveInfoContainer = new QWidget;
	tabWidget->addTab(m_saveInfoContainer, "Information");

	m_inventoryWidget = new InventoryWidget(m_gameSave);
	tabWidget->addTab(m_inventoryWidget, "Inventory");

	m_knownIngredientsWidget = new KnownIngredientsWidget(m_gameSave);
	tabWidget->addTab(m_knownIngredientsWidget, "Known ingredients effects");

	QPushButton* okButton = new QPushButton(tr("Ok"), this);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QPushButton* loadButton = new QPushButton(tr("Load save"), this);
	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadSave()));

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(loadButton);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(okButton);
	buttonsLayout->addWidget(cancelButton);
	vLayout->addLayout(buttonsLayout);

	setLayout(vLayout);

	refreshInformation();

	connect(this, SIGNAL(accepted()), this, SLOT(copySave()));
}

QSize SaveDialog::sizeHint() const
{
	return QSize(700, 500);
}

bool SaveDialog::modified() const
{
	return m_modified;
}

void SaveDialog::refreshInformation()
{
	auto l = m_saveInfoContainer->layout();
	if (l)
		QWidget().setLayout(l);
	auto layout = new QVBoxLayout(m_saveInfoContainer);

	auto loaded = m_gameSave.isLoaded();
	auto screenshotLabel = new QLabel("Not loaded");
	if (!loaded)
	{
		layout->addWidget(screenshotLabel);
		layout->addStretch();
		return;
	}

	screenshotLabel->setPixmap(m_gameSave.screenshot());
	layout->addWidget(screenshotLabel);

	auto header = m_gameSave.header();
	layout->addWidget(new QLabel(header.playerName));
	layout->addWidget(new QLabel(header.playerLocation));
	layout->addWidget(new QLabel(QString("Level %1").arg(header.playerLevel)));
	layout->addWidget(new QLabel(QString("Save #%1").arg(header.saveNumber)));

	// Information about the known ingredients
	auto knownIng = m_gameSave.knownIngredients();
	int knownEffects = 0;
	for (auto ing : knownIng)
	{
		for (int i = 0; i < 4; ++i)
			if (ing[i])
				++knownEffects;
	}

	int nbIng = knownIng.size();
	auto ingLabel = new QLabel(QString("%1 known ingredients, with %2% of discovered effects").arg(nbIng).arg(nbIng ? knownEffects * 25 / nbIng : 0));
	layout->addWidget(ingLabel);
	layout->addStretch();
}

void SaveDialog::copySave()
{
	GameSave::instance() = m_gameSave;
}

void SaveDialog::loadSave()
{
	m_modified = true;
	m_inventoryWidget->beginReset();
	m_knownIngredientsWidget->beginReset();

	m_gameSave.loadSaveFromConfig();
	refreshInformation();

	m_inventoryWidget->endReset();
	m_knownIngredientsWidget->endReset();
}
