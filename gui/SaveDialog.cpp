#include <QtWidgets>

#include "SaveDialog.h"
#include "Settings.h"

#include "ContainersWidget.h"
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

	// Settings for the save selection
	auto loadSettingsGroupBox = new QGroupBox(tr("Save selection"));
	auto loadSettingsLayout = new QVBoxLayout(loadSettingsGroupBox);

	auto useMostRecentCheckBox = new QCheckBox(tr("Use most recent save"));
	useMostRecentCheckBox->setCheckState(m_loadMostRecent ? Qt::Checked : Qt::Unchecked);
	loadSettingsLayout->addWidget(useMostRecentCheckBox);
	
	auto saveComboBox = new QComboBox;
	QStringList savesNames;
	for (auto fileInfo : m_savesList)
		savesNames.push_back(fileInfo.completeBaseName());
	saveComboBox->addItems(savesNames);
	loadSettingsLayout->addWidget(saveComboBox);

	if (!m_selectedSavePath.isEmpty()) // Set current index based on previous settings
	{
		auto name = QFileInfo(m_selectedSavePath).completeBaseName();
		auto pos = savesNames.indexOf(name);
		if (pos != -1)
			saveComboBox->setCurrentIndex(pos);
	}
	
	// Settings for the save parsing
	auto parseSettingsGroupBox = new QGroupBox(tr("Parse settings"));
	auto parseSettingsLayout = new QFormLayout(parseSettingsGroupBox);

	m_maxValidIngredientCountEdit = new QLineEdit;
	m_minValidNbIngredientsEdit = new QLineEdit;

	m_maxValidIngredientCountEdit->setText(QString::number(settings.maxValidIngredientCount));
	m_minValidNbIngredientsEdit->setText(QString::number(settings.minValidNbIngredients));

	parseSettingsLayout->addRow(tr("Maximum valid ingredient count"), m_maxValidIngredientCountEdit);
	parseSettingsLayout->addRow(tr("Minimum valid number of ingredients"), m_minValidNbIngredientsEdit);

	// Left layout
	auto leftLayout = new QVBoxLayout;
	leftLayout->addWidget(loadSettingsGroupBox);
	leftLayout->addWidget(parseSettingsGroupBox);
	leftLayout->addStretch();
	
	// Right widget
	m_saveInfoContainer = new QGroupBox(tr("Information"));
	
	// General tab layout
	auto informationLayout = new QHBoxLayout;
	informationLayout->addLayout(leftLayout);
	informationLayout->addWidget(m_saveInfoContainer);
	auto outerInformationLayout = new QVBoxLayout;
	outerInformationLayout->addLayout(informationLayout);
	outerInformationLayout->addStretch();
	auto informationWidget = new QWidget;
	informationWidget->setLayout(outerInformationLayout);
	tabWidget->addTab(informationWidget, tr("General"));

	// Other tabs
	m_knownIngredientsWidget = new KnownIngredientsWidget(m_gameSave);
	tabWidget->addTab(m_knownIngredientsWidget, tr("Known ingredients effects"));

	m_inventoryWidget = new InventoryWidget(m_gameSave.inventory()); // We keep the pointer to the vector inside GameSave
	tabWidget->addTab(m_inventoryWidget, tr("Inventory"));

	m_containersWidget = new ContainersWidget(m_gameSave);
	auto containersScrollArea = new QScrollArea(this);
	containersScrollArea->setWidget(m_containersWidget);
	containersScrollArea->setFrameStyle(QFrame::NoFrame);
	containersScrollArea->setWidgetResizable(true);
	tabWidget->addTab(containersScrollArea, tr("Containers"));

	QPushButton* okButton = new QPushButton(tr("Ok"), this);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QPushButton* loadButton = new QPushButton(tr("Reload save"), this);
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
}

void SaveDialog::copySave()
{
	GameSave::instance() = m_gameSave;
	auto& settings = Settings::instance();
	settings.loadMostRecentSave = m_loadMostRecent;
	settings.selectedSavePath = m_selectedSavePath;
	settings.maxValidIngredientCount = m_maxValidIngredientCountEdit->text().toInt();
	settings.minValidNbIngredients = m_minValidNbIngredientsEdit->text().toInt();
}

void SaveDialog::loadSave()
{
	m_modified = true;
	m_inventoryWidget->beginReset();
	m_knownIngredientsWidget->beginReset();
	m_containersWidget->clear();

	m_gameSave.setMaxValidIngredientCount(m_maxValidIngredientCountEdit->text().toInt());
	m_gameSave.setMinValidNbIngredients(m_minValidNbIngredientsEdit->text().toInt());

	if (m_loadMostRecent && !m_savesList.empty())
		m_gameSave.load(m_savesList.first().absoluteFilePath());
	else
		m_gameSave.load(m_selectedSavePath);
	refreshInformation();

	m_inventoryWidget->endReset();
	m_knownIngredientsWidget->endReset();
	m_containersWidget->refreshList();
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