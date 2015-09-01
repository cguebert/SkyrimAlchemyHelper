#pragma once

#include <QWidget>

#include <vector>
#include <cstdint>

class QVBoxLayout;

enum class FilterActionType
{ addFilterContains, addFilterDoesNotContain, RemoveFilter };

class SelectorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SelectorWidget(QWidget* parent = nullptr);

	void setItems(QStringList items);

public slots:
	void addFilterContains();
	void addFilterDoesNotContain();
	void removeFilter();

protected:
	virtual bool filterAction(FilterActionType actionType, int id) = 0;

	void parseFilterAction(FilterActionType actionType);

	QStringList m_items;
};

