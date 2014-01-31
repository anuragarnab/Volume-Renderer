#include "controlwindow.h"
#include <QtWidgets/QApplication>
#include "glWidget.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ControlWindow w;
	w.show();

	glWidget glw (256, 256, 109);
	glw.show();

	return a.exec();
}
