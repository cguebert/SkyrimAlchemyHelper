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
	using ItemTuple = std::tuple<FilterActionType, int, QWidget*>;

	bool updateExisting(std::vector<ItemTuple>& list, FilterActionType action, int id);
	void removeWidget(QWidget* widget);

	FlowLayout *m_flowLayout;
	using ItemTuple = std::tuple<FilterActionType, int, QWidget*>;
	std::vector<ItemTuple> m_effects, m_ingredients;
};
