#include <QtWidgets>

#include "SaveDialog.h"
#include "Settings.h"
#include "InventoryWidget.h"
#include "KnownIngredientsWidget.h"

SaveDialog::SaveDialog(QWidget *parent)
	: QDialog(parent)
{
	m_gameSave = GameSave::instance();
	m_savesList = m_gameSave.savesList();

	const auto& settings = Settings::instance();
	m_loadMostRecent = settings.loadMostRecentSave;
	m_selectedSavePath = settings.selectedSavePath;

	setWindowTitle(tr("Skyrim Alchemy Helper - Save information"));

	auto vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(5, 5, 5, 5);

	auto tabWidget = new QTabWidget;
	vLayout->addWidget(tabWidget);

	auto informationWidget = new QWidget;
	auto informationLayout = new QHBoxLayout(informationWidget);

	auto settingsGroupBox = new QGroupBox(tr("Settings"));
	auto settingsLayout = new QVBoxLayout;

	auto useMostRecentCheckBox = new QCheckBox(tr("Use most recent save"));
	useMostRecentCheckBox->setCheckState(m_loadMostRecent ? Qt::Checked : Qt::Unchecked);
	settingsLayout->addWidget(useMostRecentCheckBox);
	
	auto saveComboBox = new QComboBox;
	QStringList savesNames;
	for (auto fileInfo : m_savesList)
		savesNames.push_back(fileInfo.completeBaseName());
	saveComboBox->addItems(savesNames);
	settingsLayout->addWidget(saveComboBox);

	if (!m_selectedSavePath.isEmpty())
	{
		auto name = QFileInfo(m_selectedSavePath).completeBaseName();
		auto pos = savesNames.indexOf(name);
		if (pos != -1)
			saveComboBox->setCurrentIndex(pos);
	}

	settingsLayout->addStretch();
	settingsGroupBox->setLayout(settingsLayout);
	informationLayout->addWidget(settingsGroupBox);

	m_saveInfoContainer = new QGroupBox(tr("Information"));
	informationLayout->addWidget(m_saveInfoContainer);
	tabWidget->addTab(informationWidget, tr("General"));

	m_inventoryWidget = new InventoryWidget(m_gameSave);
	tabWidget->addTab(m_inventoryWidget, tr("Inventory"));

	m_knownIngredientsWidget = new KnownIngredientsWidget(m_gameSave);
	tabWidget->addTab(m_knownIngredientsWidget, tr("Known ingredients effects"));

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

	connect(useMostRecentCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLoadMostRecent(int)));
	connect(saveComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(saveSelected(int)));
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
	auto screenshotLabel = new QLabel(tr("Not loaded"));
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
	layout->addWidget(new QLabel(QString(tr("Level %1")).arg(header.playerLevel)));
	layout->addWidget(new QLabel(QString(tr("Save #%1")).arg(header.saveNumber)));

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
	auto ingLabel = new QLabel(QString(tr("%1 known ingredients, with %2% of discovered effects")).arg(nbIng).arg(nbIng ? knownEffects * 25 / nbIng : 0));
	layout->addWidget(ingLabel);
	layout->addStretch();
}

void SaveDialog::copySave()
{
	GameSave::instance() = m_gameSave;
	auto& settings = Settings::instance();
	settings.loadMostRecentSave = m_loadMostRecent;
	settings.selectedSavePath = m_selectedSavePath;
}

void SaveDialog::loadSave()
{
	m_modified = true;
	m_inventoryWidget->beginReset();
	m_knownIngredientsWidget->beginReset();

	if (m_loadMostRecent && !m_savesList.empty())
		m_gameSave.load(m_savesList.first().absoluteFilePath());
	else
		m_gameSave.load(m_selectedSavePath);
	refreshInformation();

	m_inventoryWidget->endReset();
	m_knownIngredientsWidget->endReset();
}

void SaveDialog::saveSelected(int index)
{
	m_selectedSavePath = m_savesList[index].absoluteFilePath();
	loadSave();
}

void SaveDialog::setLoadMostRecent(int state)
{
	m_loadMostRecent = (state == Qt::Checked);
	loadSave();
}