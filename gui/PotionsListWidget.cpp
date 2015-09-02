#include <QtWidgets>

#include "PotionsListWidget.h"
#include "PotionsList.h"

PotionsListWidget::PotionsListWidget(QWidget* parent)
	: QFrame(parent)
{
	setFrameShape(QFrame::StyledPanel);
	PotionsList::instance().setUpdateCallback([this]() { refreshList(); });
	refreshList();
}

void PotionsListWidget::refreshList()
{
	auto l = layout();
	if (l)
		QWidget().setLayout(l);

	const auto& potions = PotionsList::instance().filteredPotions();

	QVBoxLayout* vLayout = new QVBoxLayout;

	auto label = new QLabel("# of potions : " + QString::number(potions.size()));
	vLayout->addWidget(label);
	vLayout->addStretch();

	setLayout(vLayout);
}