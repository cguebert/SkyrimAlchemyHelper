#pragma once

#include <QMainWindow>

class EffectsSelector;
class IngredientsSelector;
class FiltersWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);

	QSize sizeHint() const;

signals:
	void mainWindowShown();

public slots:
	void editConfig();
	void afterLaunch();
	void gameSaveInformation();

protected:
	void showEvent(QShowEvent* event);
	void closeEvent(QCloseEvent* event);

	void readSettings();
	void writeSettings();
	
	EffectsSelector *m_effectsSelector;
	IngredientsSelector *m_ingredientsSelector;
	FiltersWidget *m_filtersWidget;
};
