#include <QtWidgets>

#include "IngredientsListDelegate.h"
#include "EffectsList.h"

IngredientsListDelegate::IngredientsListDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
{

}

void IngredientsListDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QStyledItemDelegate::setEditorData(editor, index);

	if(!((index.column() - 2) % 3)) // 2, 5, 8, 11
	{
		QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
		if(lineEdit)
		{
			QStringList effectsName;
			static const EffectsList& effects = EffectsList::GetInstance();
			for(const auto& effect : effects.effects())
				effectsName << effect.name;

			auto completer = new QCompleter(effectsName, editor);
			completer->setCaseSensitivity(Qt::CaseInsensitive);
			lineEdit->setCompleter(completer);
		}
	}
}
