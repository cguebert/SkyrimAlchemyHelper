#include <QtWidgets>

#include "SaveDialog.h"
#include "Settings.h"

#include "Config.h"
#include "ContainersWidget.h"
#include "InventoryWidget.h"
#include "KnownIngredientsWidget.h"
#include "ModsParserWrapper.h"

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
	
	m_saveComboBox = new QComboBox;
	for (auto fileInfo : m_savesList)
		m_savesNames.push_back(fileInfo.completeBaseName());
	m_saveComboBox->addItems(m_savesNames);
	loadSettingsLayout->addWidget(m_saveComboBox);

	if (m_loadMostRecent)
	{
		m_saveComboBox->setCurrentIndex(0);
		m_saveComboBox->setEnabled(false);
	}
	else if (!m_selectedSavePath.isEmpty()) // Set current index based on previous settings
		selectCurrentSave();
	
	// Settings for the save parsing
	auto parseSettingsGroupBox = new QGroupBox(tr("Parse settings"));
	auto parseSettingsLayout = new QFormLayout(parseSettingsGroupBox);

	m_maxValidIngredientCountEdit = new QLineEdit;
	m_maxValidIngredientCountEdit->setText(QString::number(settings.maxValidIngredientCount));
	m_maxValidIngredientCountEdit->setToolTip(tr("Consider invalid an ingredient that has a count greater than this number."));
	parseSettingsLayout->addRow(tr("Maximum valid ingredient count"), m_maxValidIngredientCountEdit);

	m_minValidNbIngredientsEdit = new QLineEdit;
	m_minValidNbIngredientsEdit->setText(QString::number(settings.minValidNbIngredients));
	m_minValidNbIngredientsEdit->setToolTip(tr("Do not keep containers that have less than this number of type of ingredients."));
	parseSettingsLayout->addRow(tr("Minimum valid number of ingredients types"), m_minValidNbIngredientsEdit);

	m_minTotalIngredientsCountEdit = new QLineEdit;
	m_minTotalIngredientsCountEdit->setText(QString::number(settings.minTotalIngredientsCount));
	m_minTotalIngredientsCountEdit->setToolTip(tr("Do not keep containers that have less than this total number of ingredients of all types."));
	parseSettingsLayout->addRow(tr("Minimum total number of ingredients"), m_minTotalIngredientsCountEdit);

	// Settings for filtering containers based on their id
	auto containersFiltersGroupBox = new QGroupBox(tr("Filter containers"));
	auto containersFiltersLayout = new QFormLayout(containersFiltersGroupBox);

	m_playerOnlyCheckBox = new QCheckBox;
	m_playerOnlyCheckBox->setCheckState(settings.playerOnly ? Qt::Checked : Qt::Unchecked);
	m_playerOnlyCheckBox->setToolTip(tr("Ignore all containers."));
	containersFiltersLayout->addRow(tr("Only get the player's inventory"), m_playerOnlyCheckBox);

	m_getContainersNamesCheckBox = new QCheckBox;
	m_getContainersNamesCheckBox->setCheckState(settings.getContainersInfo ? Qt::Checked : Qt::Unchecked);
	m_getContainersNamesCheckBox->setToolTip(tr("Reparse the mods to extract the containers name and location (can be long)."));
	containersFiltersLayout->addRow(tr("Get the containers' names"), m_getContainersNamesCheckBox);

	m_sameCellAsPlayerCheckBox = new QCheckBox;
	m_sameCellAsPlayerCheckBox->setCheckState(settings.sameCellAsPlayer ? Qt::Checked : Qt::Unchecked);
	m_sameCellAsPlayerCheckBox->setToolTip(tr("Only keep the containers in the same cell as the player."));
	m_sameCellAsPlayerCheckBox->setEnabled(settings.getContainersInfo);
	m_sameCellAsPlayerLabel = new QLabel(tr("Containers in same cell as player"));
	m_sameCellAsPlayerLabel->setEnabled(settings.getContainersInfo);
	containersFiltersLayout->addRow(m_sameCellAsPlayerLabel, m_sameCellAsPlayerCheckBox);

	m_interiorCellsOnlyCheckBox = new QCheckBox;
	m_interiorCellsOnlyCheckBox->setCheckState(settings.interiorCellsOnly ? Qt::Checked : Qt::Unchecked);
	m_interiorCellsOnlyCheckBox->setToolTip(tr("Only keep the containers that are in interior cells."));
	m_interiorCellsOnlyCheckBox->setEnabled(settings.getContainersInfo);
	m_interiorCellsOnlyLabel = new QLabel(tr("Interior cells only"));
	m_interiorCellsOnlyLabel->setEnabled(settings.getContainersInfo);
	containersFiltersLayout->addRow(m_interiorCellsOnlyLabel, m_interiorCellsOnlyCheckBox);

	// Left layout
	auto leftLayout = new QVBoxLayout;
	leftLayout->addWidget(loadSettingsGroupBox);
	leftLayout->addWidget(parseSettingsGroupBox);
	leftLayout->addWidget(containersFiltersGroupBox);
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

	if (settings.getContainersInfo)
		refreshContainersNames();

	connect(this, SIGNAL(accepted()), this, SLOT(copySave()));

	connect(useMostRecentCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLoadMostRecent(int)));
	connect(m_saveComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(saveSelected(int)));
	connect(m_getContainersNamesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(getContainersNamesChanged(int)));
	connect(m_containersWidget, SIGNAL(containersStatusUpdated()), this, SLOT(updateIngredientsCount()));
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
	int knownEffects = 0, knownIngredients = 0;
	for (auto ing : knownIng)
	{
		bool known = false;
		for (int i = 0; i < 4; ++i)
			if (ing[i])
			{
				++knownEffects;
				known = true;
			}

		if (known)
			++knownIngredients;
	}

	int nbIng = knownIng.size();
	auto ingLabel = new QLabel(QString(tr("%1 known ingredients, with %2% of discovered effects"))
		.arg(knownIngredients).arg(knownIngredients ? knownEffects * 25 / knownIngredients : 0));
	layout->addWidget(ingLabel);

	// Information about the inventory
	int total = 0;
	for (auto ing : m_gameSave.ingredientsCount())
		total += ing;
	int nbActiveContainers = 0;
	for (const auto status : m_gameSave.containersState())
		if (status)
			++nbActiveContainers;
	layout->addWidget(new QLabel(QString(tr("%1 total ingredients in %2 containers"))
		.arg(total).arg(nbActiveContainers)));
}

void SaveDialog::copySave()
{
	GameSave::instance() = m_gameSave;
	auto& settings = Settings::instance();
	settings.loadMostRecentSave = m_loadMostRecent;
	settings.selectedSavePath = m_selectedSavePath;
	settings.maxValidIngredientCount = m_maxValidIngredientCountEdit->text().toInt();
	settings.minValidNbIngredients = m_minValidNbIngredientsEdit->text().toInt();
	settings.minTotalIngredientsCount = m_minTotalIngredientsCountEdit->text().toInt();
	settings.playerOnly = m_playerOnlyCheckBox->checkState() == Qt::Checked;
	settings.getContainersInfo = m_getContainersNamesCheckBox->checkState() == Qt::Checked;
	settings.sameCellAsPlayer = m_sameCellAsPlayerCheckBox->checkState() == Qt::Checked;
	settings.interiorCellsOnly = m_interiorCellsOnlyCheckBox->checkState() == Qt::Checked;
}

void SaveDialog::loadSave()
{
	m_modified = true;
	m_inventoryWidget->beginReset();
	m_knownIngredientsWidget->beginReset();
	m_containersWidget->clear();

	m_gameSave.setMaxValidIngredientCount(m_maxValidIngredientCountEdit->text().toInt());
	m_gameSave.setMinValidNbIngredients(m_minValidNbIngredientsEdit->text().toInt());
	m_gameSave.setMinTotalIngredientsCount(m_minTotalIngredientsCountEdit->text().toInt());
	m_gameSave.setPlayerOnly(m_playerOnlyCheckBox->checkState() == Qt::Checked);
	if (m_getContainersNamesCheckBox->checkState() == Qt::Checked)
	{
		m_gameSave.setFilterSameCellAsPlayer(m_sameCellAsPlayerCheckBox->checkState() == Qt::Checked);
		m_gameSave.setInteriorCellsOnly(m_interiorCellsOnlyCheckBox->checkState() == Qt::Checked);
	}
	else
	{
		m_gameSave.setFilterSameCellAsPlayer(false);
		m_gameSave.setInteriorCellsOnly(false);
	}

	if (m_loadMostRecent && !m_savesList.empty())
		m_gameSave.load(m_savesList.first().absoluteFilePath());
	else
		m_gameSave.load(m_selectedSavePath);
	refreshInformation();
	if (m_getContainersNamesCheckBox->checkState() == Qt::Checked)
		refreshContainersNames();

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
	bool checked = (state == Qt::Checked);
	m_loadMostRecent = checked;
	m_saveComboBox->setEnabled(!checked);

	if (!checked)
		selectCurrentSave();
	else
	{
		QSignalBlocker blocker(m_saveComboBox);
		m_saveComboBox->setCurrentIndex(0);
	}

	loadSave();
}

void SaveDialog::getContainersNamesChanged(int state)
{
	bool enabled = (state == Qt::Checked);
	m_sameCellAsPlayerCheckBox->setEnabled(enabled);
	m_sameCellAsPlayerLabel->setEnabled(enabled);
	m_interiorCellsOnlyCheckBox->setEnabled(enabled);
	m_interiorCellsOnlyLabel->setEnabled(enabled);
}

void SaveDialog::containersUpdated()
{
	if (m_sameCellAsPlayerCheckBox->checkState() == Qt::Checked)
	{
		m_inventoryWidget->beginReset();
		m_gameSave.filterContainers();
		m_inventoryWidget->endReset();

		refreshInformation();
		m_containersWidget->updateCheckBoxes();
	}
		
	m_containersWidget->updateIdLabels();
}

void SaveDialog::updateIngredientsCount()
{
	m_inventoryWidget->beginReset();

	m_gameSave.computeIngredientsCount();

	m_inventoryWidget->endReset();
	refreshInformation();
}

void SaveDialog::selectCurrentSave()
{
	QSignalBlocker blocker(m_saveComboBox);
	auto name = QFileInfo(m_selectedSavePath).completeBaseName();
	auto pos = m_savesNames.indexOf(name);
	if (pos != -1)
		m_saveComboBox->setCurrentIndex(pos);
	else
		m_saveComboBox->setCurrentIndex(0);
}

void SaveDialog::refreshContainersNames()
{
	const auto& saveContainers = m_gameSave.containers();
	std::vector<uint32_t> saveIds, configIds, updateIds;
	for (const auto& c : saveContainers)
	{
		if (c.id != 0x14)
			saveIds.push_back(c.id);
	}

	{
		std::lock_guard<std::mutex> lock(ContainersCache::instance().containersMutex);
		const auto& configContainers = ContainersCache::instance().containers;
		for (const auto& c : configContainers)
			configIds.push_back(c.code);
	}

	std::sort(saveIds.begin(), saveIds.end());
	std::sort(configIds.begin(), configIds.end());

	std::set_difference(saveIds.begin(), saveIds.end(), configIds.begin(), configIds.end(), std::back_inserter(updateIds));
	if (updateIds.empty())
		return;

	auto workerThread = new ContainersWorkerThread(updateIds, this);
	connect(workerThread, SIGNAL(resultReady()), this, SLOT(containersUpdated()));
	connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
	workerThread->start();
}

void ContainersWorkerThread::run()
{
	ModsParserWrapper parser;
	if (parser.updateContainers(m_ids) != ModsParserWrapper::Result::Success)
		return;

	const auto& parserContainers = parser.containers();
	{
		std::lock_guard<std::mutex> lock(ContainersCache::instance().containersMutex);
		auto& configContainers = ContainersCache::instance().containers;
		configContainers.insert(configContainers.end(), parserContainers.begin(), parserContainers.end());
	}
	ContainersCache::instance().save();

	emit resultReady();
}
