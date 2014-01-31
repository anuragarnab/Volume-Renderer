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
	QObject::connect(&w, SIGNAL(volRendConfigFile(QString)), &r, SLOT(initVolRenderer(QString)));

/*	glWidget glw(256, 256, 109);
//	glw.resize(256, 256);

	QHBoxLayout mainLayout;
	mainLayout.addWidget(&glw);
	mainLayout.addWidget(&r);
	QWidget window;

	window.setLayout(&mainLayout);
	window.show();*/

	/*glWidget glw (256, 256, 109);

	r.addVolumeRenderer(&glw);*/

	return a.exec();
}
