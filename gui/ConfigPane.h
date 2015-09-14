#ifndef CONFIGPANE_H
#define CONFIGPANE_H

#include <QWidget>

class QCheckBox;
class QLineEdit;
class QPushButton;

class IngredientsList;
class EffectsList;
class PluginsList;

namespace modParser { struct Config; }

class ConfigPane : public QWidget
{
	Q_OBJECT

public:
	ConfigPane(IngredientsList& ingredientsList,
		EffectsList& effectsList,
		PluginsList& pluginsList,
		bool firstLaunch = false,
		QWidget* parent = nullptr);

	bool testConfig(); // Returns false if there is a problem with the current configuration
	void saveConfig();
	bool modified();

public slots:
	void editDataPath();
	void editPluginsPath();
	void editSavesPath();
	void editModOrganizerPath();
	void useModOrganizerChanged(int);
	void parseMods();
	void defaultConfig();

signals:
	void startModsParse();
	void endModsParse();

protected:
	void loadConfig();
	void convertConfig(const modParser::Config& config);
	bool getModsPaths(std::vector<std::string>& paths);
	bool findRealPaths(std::vector<std::string>& paths);

	bool m_firstLaunch;
	QLineEdit *m_dataFolderEdit,
		*m_pluginsListPathEdit, 
		*m_savesFolderEdit, 
		*m_modOrganizerPathEdit,
		*m_languageEdit;
	QCheckBox *m_useModOrganizerCheckBox;
	QPushButton *m_modOrganizerPathButton;
	bool m_modified = false;

	IngredientsList& m_ingredientsList;
	EffectsList& m_effectsList;
	PluginsList& m_pluginsList;
};

#endif // CONFIGPANE_H
