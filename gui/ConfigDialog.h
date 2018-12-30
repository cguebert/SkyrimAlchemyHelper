/******************************************************************************
*                            Skyrim Alchemy Helper                            *
*******************************************************************************
*                                                                             *
* Copyright (C) 2015 Christophe Guebert                                       *
*                                                                             *
* This program is free software; you can redistribute it and/or modify        *
* it under the terms of the GNU General Public License as published by        *
* the Free Software Foundation; either version 2 of the License, or           *
* (at your option) any later version.                                         *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc.,     *
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                 *
*******************************************************************************
*                        Skyrim Alchemy Helper :: Gui                         *
*                                                                             *
*                  Contact: christophe.guebert+SAH@gmail.com                  *
******************************************************************************/
#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

#include "Config.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QTabWidget;

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog(QWidget* parent = nullptr);

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
	void defaultConfig(bool firstLaunch = false);
	void gameChanged(QString gameName);

signals:
	void startModsParse();
	void endModsParse();

protected:
	QWidget* createConfigPane();
	bool testConfig(); // Returns false if there is a problem with the current configuration
	void saveConfig();
	void loadConfig();

	void saveSettings(QString gameName);
	void loadSettings(QString gameName);

	QLineEdit *m_dataFolderEdit,
		*m_pluginsListPathEdit,
		*m_savesFolderEdit,
		*m_modOrganizerPathEdit;
	QComboBox *m_gameNameComboBox,
		*m_languageComboBox;
	QCheckBox* m_useModOrganizerCheckBox;
	QPushButton* m_modOrganizerPathButton;
	QString m_currentGame;
	bool m_modified = false;

	Config m_config;
};

#endif // CONFIGDIALOG_H
