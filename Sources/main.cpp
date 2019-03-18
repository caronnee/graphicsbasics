#include "tracinggui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TracingGui w;
	w.show();
	return a.exec();
}
