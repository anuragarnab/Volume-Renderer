#include "busywindow.h"

BusyWindow::BusyWindow(QWidget *parent) :
QWidget(parent)
{

	label = new QLabel("Please wait whilst the texture loads ...");

	vbox.addWidget(label);
	setLayout(&vbox);

	setWindowTitle("Busy ...");
	setMinimumSize(200, 100);
}

BusyWindow::~BusyWindow()
{
}
