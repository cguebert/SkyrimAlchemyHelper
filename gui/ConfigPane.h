#ifndef CONFIGPANE_H
#define CONFIGPANE_H

#include <QWidget>

class QCheckBox;
class QLineEdit;
class QPushButton;

class Config;

namespace modParser { struct Config; }

class ConfigPane : public QWidget
{
	Q_OBJECT

public:
	ConfigPane(Config& config,
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

	bool m_firstLaunch;
	QLineEdit *m_dataFolderEdit,
		*m_pluginsListPathEdit, 
		*m_savesFolderEdit, 
		*m_modOrganizerPathEdit,
		*m_languageEdit;
	QCheckBox *m_useModOrganizerCheckBox;
	QPushButton *m_modOrganizerPathButton;
	bool m_modified = false;

	Config& m_config;
};

#endif // CONFIGPANE_H
