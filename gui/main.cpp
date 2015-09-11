#include "MainWindow.h"
#include <QApplication>

#include "EffectsList.h"
#include "GameSave.h"
#include "IngredientsList.h"
#include "PluginsList.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Christophe Guebert");
	app.setApplicationName("SkyrimAlchemyHelper");

	// The order is important
	PluginsList::instance().loadList();
	EffectsList::instance().loadList();
	IngredientsList::instance().loadList();
	GameSave::instance().loadSaveFromConfig();

	MainWindow window;
	window.show();

	return app.exec();
}
