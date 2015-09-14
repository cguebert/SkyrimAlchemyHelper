#include "MainWindow.h"
#include <QApplication>

#include "Config.h"
#include "GameSave.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Christophe Guebert");
	app.setApplicationName("SkyrimAlchemyHelper");

	// The order is important
	Config::main().load();
	GameSave::instance().loadSaveFromConfig();

	MainWindow window;
	window.show();

	return app.exec();
}
