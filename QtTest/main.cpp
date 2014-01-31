#include "controlwindow.h"
#include <QtWidgets/QApplication>
#include "glWidget.h"
#include "renderwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ControlWindow w;
	w.show();

	RenderWindow r;
	r.show();

	QObject::connect(&w, SIGNAL(imageFilename(QString)), &r, SLOT(initialiseImages(QString)));

	glWidget glw (256, 256, 109);
	glw.show();

	return a.exec();
}
