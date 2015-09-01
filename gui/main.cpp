#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Christophe Guebert");
	app.setApplicationName("SkyrimAlchemyHelper");

	MainWindow window;
	window.show();

	return app.exec();
}
