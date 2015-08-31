#include "SAHDialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Christophe Guebert");
	app.setApplicationName("SkyrimAlchemyHelper");

	SAHDialog dlg;
	return dlg.exec();
}
