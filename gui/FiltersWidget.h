#pragma once

#include <QWidget>

#include <vector>

enum class FilterActionType
{ addFilterContains, addFilterDoesNotContain, RemoveFilter };

class FlowLayout;

class FiltersWidget : public QWidget
{
	Q_OBJECT
public:
	explicit FiltersWidget(QWidget* parent = nullptr);

	void clear();

public slots:
	void effectFilterAction(FilterActionType action, int id);
	void ingredientFilterAction(FilterActionType action, int id);
	void removeEffect();
	void removeIngredient();

protected:
	struct FilterItem
	{
		FilterItem() {}
		FilterItem(FilterActionType a, int i, QWidget* w) : actionType(a), id(i), widget(w) {}

		FilterActionType actionType;
		int id;
		QWidget* widget;
	};

	bool updateExisting(std::vector<FilterItem>& list, FlowLayout *layout, FilterActionType action, int id);
	void removeWidget(FlowLayout *layout, QWidget* widget);
	void updatePotionsListFilters();

	FlowLayout *m_effectsLayout, *m_ingredientsLayout;
	std::vector<FilterItem> m_effectsFilters, m_ingredientsFilters;
};
