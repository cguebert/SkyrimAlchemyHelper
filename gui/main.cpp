#include "SAHDialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SAHDialog w;
	w.show();

	return a.exec();
}
