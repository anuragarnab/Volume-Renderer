/*
*	main.cpp
*	Anurag Arnab
*	23 January 2013
*
*	Driver program for the Qt based GUI
*/

#include "controlwindow.h"
#include <QtWidgets/QApplication>
#include "glWidget.h"
#include "renderwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ControlWindow w;
	w.show();
	w.move(w.pos().x()-250, w.pos().y() - 0);

	RenderWindow r;
	r.show();

	QObject::connect(&w, SIGNAL(imageFilename(QString)), &r, SLOT(initialiseImages(QString)));
	QObject::connect(&w, SIGNAL(volRendConfigFile(QString)), &r, SLOT(initVolRenderer(QString)));
	QObject::connect(&w, SIGNAL(imageFilterChosen(int, int)), &r, SLOT(getProcessOption(int, int)));
	QObject::connect(&w, SIGNAL(closeWindow(void)), &r, SLOT(forceClose(void)));
	QObject::connect(&r, SIGNAL(closeWindow(void)), &w, SLOT(forceClose(void)));
	QObject::connect(w.getLineThreshold(), SIGNAL(textChanged(QString)), &r, SLOT(getAlphaThresh(QString)));
	QObject::connect(&w, SIGNAL(alphaScale(QString)), &r, SLOT(getAlphaScale(QString)));


	return a.exec();
}
