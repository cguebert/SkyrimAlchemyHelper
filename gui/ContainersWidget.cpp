#include <QtWidgets>

#include "ContainersWidget.h"
#include "IngredientsList.h"
#include "GameSave.h"

ContainersWidget::ContainersWidget(GameSave& gameSave, QWidget* parent)
	: QFrame(parent)
	, m_gameSave(gameSave)
{
	setFrameShape(QFrame::StyledPanel);

	refreshList();
}

void ContainersWidget::refreshList()
{
	auto l = layout();
	if (l)
		QWidget().setLayout(l);

	const auto& ingredients = IngredientsList::instance().ingredients();
	const auto& containers = m_gameSave.containers();
	int nbContainers = containers.size();
	
	QVBoxLayout* vLayout = new QVBoxLayout;
	QString containersCountText;
	if (!nbContainers)
		containersCountText = tr("No containers corresponding to these parameters");
	else
		containersCountText = tr("Showing all %1 containers").arg(nbContainers);
	auto label = new QLabel(containersCountText);
	vLayout->addWidget(label);

	for (const auto& container : containers)
	{
		auto containerWidget = new QFrame;
		containerWidget->setFrameShape(QFrame::Box);
		auto containerLayout = new QVBoxLayout(containerWidget);

		auto idLabel = new QLabel(QString::number(container.id, 16).toUpper());
		auto nbIngredientsLabel = new QLabel(tr("%1 ingredients").arg(container.inventory.size()));
		
		containerLayout->addWidget(idLabel);
		containerLayout->addWidget(nbIngredientsLabel);

		vLayout->addWidget(containerWidget);
	}

	vLayout->addStretch();

	setLayout(vLayout);
}