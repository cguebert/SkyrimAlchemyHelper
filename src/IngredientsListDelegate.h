#ifndef INGREDIENTSLISTDELEGATE_H
#define INGREDIENTSLISTDELEGATE_H

#include <QStyledItemDelegate>

class IngredientsListDelegate : public QStyledItemDelegate
{
public:
	IngredientsListDelegate(QObject* parent = nullptr);

	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
};

#endif // INGREDIENTSLISTDELEGATE_H
