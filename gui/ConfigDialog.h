#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

#include "Config.h"

class QCheckBox;
class QLineEdit;
class QPushButton;
class QTabWidget;

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog(QWidget *parent = nullptr, bool firstLaunch = false);

	QSize sizeHint() const;
	bool modified() const;

public slots:
	void onOk();
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
	QWidget* createConfigPane();
	bool testConfig(); // Returns false if there is a problem with the current configuration
	void saveConfig();
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

	Config m_config;
};

#endif // CONFIGDIALOG_H
