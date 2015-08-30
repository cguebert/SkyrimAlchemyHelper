#ifndef CONFIGPANE_H
#define CONFIGPANE_H

#include <QWidget>

class QLineEdit;
class QPushButton;

class ConfigPane : public QWidget
{
	Q_OBJECT

public:
	ConfigPane(QWidget* parent = 0);

public slots:
	void editDataPath();
	void editPluginsPath();
	void editSavesPath();
	void editModOrganizerPath();
	void useModOrganizerChanged(int);
	void parseMods();
	void saveConfig();

protected:
	void loadConfig();

	QLineEdit *m_dataPathEdit,
		*m_pluginsListPathEdit, 
		*m_savesPathEdit, 
		*m_modOrganizerPathEdit;
	QPushButton *m_modOrganizerPathButton;
};

#endif // CONFIGPANE_H
