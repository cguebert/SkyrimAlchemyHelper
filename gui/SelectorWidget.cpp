#include <QtWidgets>

#include "SelectorWidget.h"

SelectorWidget::SelectorWidget(QWidget* parent) 
	: QWidget(parent)
{
}

void SelectorWidget::setItems(ItemList&& list)
{
	m_items = std::move(list);

	auto layout = this->layout();
	if (layout)
		QWidget().setLayout(layout);

	layout = new QVBoxLayout(this);

	for (const auto& item : m_items)
	{
		auto label = new QLabel(item.second);
		layout->addWidget(label);
	}
}

void SelectorWidget::setFilterContains()
{

}

void SelectorWidget::setFilterDoesNotContain()
{

}

void SelectorWidget::removeFilter()
{

}
