#include <QtWidgets>

#include "ContainersWidget.h"
#include "Config.h"
#include "InventoryWidget.h"
#include "GameSave.h"

ContainersWidget::ContainersWidget(GameSave& gameSave, QWidget* parent)
	: QFrame(parent)
	, m_gameSave(gameSave)
{
	setFrameShape(QFrame::StyledPanel);

	refreshList();
}

void ContainersWidget::clear()
{
	auto l = layout();
	if (l)
		QWidget().setLayout(l);
}

void ContainersWidget::refreshList()
{
	clear();

	const auto& ingredients = Config::main().ingredients;
	const auto& containers = m_gameSave.containers();
	const auto& containersState = m_gameSave.containersState();
	int nbContainers = containers.size();

	m_containersWidgets.clear();
	
	QVBoxLayout* vLayout = new QVBoxLayout;
	QString containersCountText;
	if (!nbContainers)
		containersCountText = tr("No containers corresponding to these parameters");
	else if (nbContainers == 1)
		containersCountText = tr("Showing 1 container");
	else
		containersCountText = tr("Showing %1 containers").arg(nbContainers);
	auto label = new QLabel(containersCountText);
	vLayout->addWidget(label);

	for (int i = 0; i < nbContainers; ++i)
	{
		const auto& container = containers[i];
		auto containerWidget = new QFrame;
		containerWidget->setFrameShape(QFrame::Box);
		
		auto idLabel = new QLabel(QString::number(container.id, 16).toUpper());
		idLabel->setMinimumWidth(60);

		int nbIng = 0;
		for (const auto& item : container.inventory)
			nbIng += item.second;

		auto nbIngredientsLabel = new QLabel(tr("%1 ingredients").arg(nbIng));
		nbIngredientsLabel->setMinimumWidth(80);
		auto inventoryWidget = new InventoryWidget(container.inventory);
		inventoryWidget->setMinimumHeight(std::min(25 + nbIng * 30, 250));
		inventoryWidget->layout()->setContentsMargins(0, 0, 0, 0);
		inventoryWidget->hide();

		auto toggleInventoryAction = new QAction(containerWidget);
		toggleInventoryAction->setData(i);
		auto toggleInventoryButton = new QPushButton(tr("Show"));
		connect(toggleInventoryButton, SIGNAL(clicked(bool)), toggleInventoryAction, SLOT(trigger()));
		connect(toggleInventoryAction, SIGNAL(triggered(bool)), this, SLOT(toggleInventoryWidget()));

		auto activeCheckBox = new QCheckBox;
		activeCheckBox->setToolTip(tr("Add this container to the ingredients available for potions."));
		activeCheckBox->setCheckState(containersState[i] ? Qt::Checked : Qt::Unchecked);
		auto toggleActiveAction = new QAction(containerWidget);
		toggleActiveAction->setData(i);
		connect(activeCheckBox, SIGNAL(stateChanged(int)), toggleActiveAction, SLOT(trigger()));
		connect(toggleActiveAction, SIGNAL(triggered(bool)), this, SLOT(toggleContainerStatus()));

		auto topLayout = new QHBoxLayout;
		topLayout->setSpacing(20);
		topLayout->addWidget(activeCheckBox);
		topLayout->addWidget(toggleInventoryButton);
		topLayout->addWidget(nbIngredientsLabel);
		topLayout->addWidget(idLabel);
		topLayout->addStretch();
		
		auto containerLayout = new QVBoxLayout(containerWidget);
		containerLayout->addLayout(topLayout);
		containerLayout->addWidget(inventoryWidget);

		vLayout->addWidget(containerWidget);

		ContainerWidgets widgets;
		widgets.id = container.id;
		widgets.activeCheckBox = activeCheckBox;
		widgets.toggleButton = toggleInventoryButton;
		widgets.idLabel = idLabel;
		widgets.inventoryWidget = inventoryWidget;
		m_containersWidgets.push_back(widgets);
	}

	vLayout->addStretch();

	updateIdLabels();

	setLayout(vLayout);
}

void ContainersWidget::toggleInventoryWidget()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
	{
		bool ok = false;
		int id = action->data().toInt(&ok);
		if (ok)
		{
			auto& widgets = m_containersWidgets[id];
			if (widgets.inventoryWidget->isVisible())
			{
				widgets.inventoryWidget->hide();
				widgets.toggleButton->setText(tr("Show"));
			}
			else
			{
				widgets.inventoryWidget->show();
				widgets.toggleButton->setText(tr("Hide"));
			}
		}
	}
}

QString ContainersWidget::getContainerLabel(quint32 id)
{
	if (id == 0x14)
		return tr("Player");

	const auto& containers = ContainersCache::instance().containers;
	auto it = std::find_if(containers.begin(), containers.end(), [id](const ContainersCache::Container& c) {
		return c.code == id;
	});

	if (it != containers.end())
		return tr("%1 in %2").arg(it->name).arg(it->location);

	return QString::number(id, 16).toUpper();
}

void ContainersWidget::updateIdLabels()
{
	std::lock_guard<std::mutex> lock(ContainersCache::instance().containersMutex);
	for (auto& widgets : m_containersWidgets)
		widgets.idLabel->setText(getContainerLabel(widgets.id));
}

void ContainersWidget::updateCheckBoxes()
{
	const auto& containersState = m_gameSave.containersState();
	for (int i = 0, nb = m_containersWidgets.size(); i < nb; ++i)
	{
		QSignalBlocker blocker(m_containersWidgets[i].activeCheckBox);
		m_containersWidgets[i].activeCheckBox->setChecked(containersState[i] ? Qt::Checked : Qt::Unchecked);
	}
}

void ContainersWidget::toggleContainerStatus()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
	{
		bool ok = false;
		int id = action->data().toInt(&ok);
		if (ok)
		{
			auto& containersState = m_gameSave.containersState();
			auto& widgets = m_containersWidgets[id];
			containersState[id] = widgets.activeCheckBox->checkState() == Qt::Checked;
			emit containersStatusUpdated();
		}
	}
}
