#pragma once

#include <QWidget>

#include <vector>
#include <cstdint>

class QVBoxLayout;

class SelectorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SelectorWidget(QWidget* parent = nullptr);

	using ItemPair = std::pair<int32_t, QString>;
	using ItemList = std::vector<ItemPair>;

	void setItems(ItemList&& list);

public slots:
	void setFilterContains();
	void setFilterDoesNotContain();
	void removeFilter();

protected:
	virtual bool setFilterContains(int32_t id) = 0;
	virtual bool setFilterDoesNotContain(int32_t id) = 0;
	virtual bool removeFilter(int32_t id) = 0;

	ItemList m_items;
};

