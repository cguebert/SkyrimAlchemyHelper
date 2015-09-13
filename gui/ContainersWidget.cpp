#include <QtWidgets>

#include "ContainersWidget.h"
#include "IngredientsList.h"
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

	const auto& ingredients = IngredientsList::instance().ingredients();
	const auto& containers = m_gameSave.containers();
	int nbContainers = containers.size();

	m_toggleButtons.clear();
	m_inventoryWidgets.clear();
	
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
		auto nbIngredientsLabel = new QLabel(tr("%1 ingredients").arg(nbIng));
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
		topLayout->addWidget(idLabel);
		topLayout->addWidget(nbIngredientsLabel);
		topLayout->addStretch();
		
		auto containerLayout = new QVBoxLayout(containerWidget);
		containerLayout->addLayout(topLayout);
		containerLayout->addWidget(inventoryWidget);

		vLayout->addWidget(containerWidget);
	}

	vLayout->addStretch();

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