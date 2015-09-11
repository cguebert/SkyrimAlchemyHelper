#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

#include "EffectsList.h"
#include "IngredientsList.h"
#include "PluginsList.h"

class ConfigPane;

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog(QWidget *parent = nullptr, bool firstLaunch = false);

	QSize sizeHint() const;
	bool areListsModified() const;

public slots:
	void onOk();

protected:
	void copyLists();

	ConfigPane* m_configPane;

	EffectsList m_effectsList;
	IngredientsList m_ingredientsList;
	PluginsList m_pluginsList;
};

#endif // CONFIGDIALOG_H
