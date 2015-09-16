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
	int nbContainers = containers.size();

	m_toggleButtons.clear();
	m_inventoryWidgets.clear();
	m_idLabels.clear();
	
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
		
		int nbIng = container.inventory.size();
		auto idLabel = new QLabel(QString::number(container.id, 16).toUpper());
		idLabel->setMinimumWidth(60);
		m_idLabels.emplace_back(idLabel, container.id);

		auto nbIngredientsLabel = new QLabel(tr("%1 ingredients").arg(nbIng));
		nbIngredientsLabel->setMinimumWidth(80);
		auto inventoryWidget = new InventoryWidget(container.inventory);
		inventoryWidget->setMinimumHeight(std::min(25 + nbIng * 30, 250));
		inventoryWidget->layout()->setContentsMargins(0, 0, 0, 0);
		inventoryWidget->hide();
		m_inventoryWidgets.push_back(inventoryWidget);

		auto toggleInventoryAction = new QAction(containerWidget);
		toggleInventoryAction->setData(i);
		auto toggleInventoryButton = new QPushButton(tr("Show"));
		connect(toggleInventoryButton, SIGNAL(clicked(bool)), toggleInventoryAction, SLOT(trigger()));
		connect(toggleInventoryAction, SIGNAL(triggered(bool)), this, SLOT(toggleInventoryWidget()));
		m_toggleButtons.push_back(toggleInventoryButton);

		auto topLayout = new QHBoxLayout;
		topLayout->setSpacing(20);
		topLayout->addWidget(toggleInventoryButton);
		topLayout->addWidget(nbIngredientsLabel);
		topLayout->addWidget(idLabel);
		topLayout->addStretch();
		
		auto containerLayout = new QVBoxLayout(containerWidget);
		containerLayout->addLayout(topLayout);
		containerLayout->addWidget(inventoryWidget);

		vLayout->addWidget(containerWidget);
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
			if (m_inventoryWidgets[id]->isVisible())
			{
				m_inventoryWidgets[id]->hide();
				m_toggleButtons[id]->setText(tr("Show"));
			}
			else
			{
				m_inventoryWidgets[id]->show();
				m_toggleButtons[id]->setText(tr("Hide"));
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
	for (auto& idLabel : m_idLabels)
		idLabel.first->setText(getContainerLabel(idLabel.second));
}