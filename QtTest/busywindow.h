#ifndef BUSYWINDOW_H
#define BUSYWINDOW_H

#include <QtWidgets>

class BusyWindow : public QWidget
{
	Q_OBJECT

public:
	BusyWindow(QWidget *parent = 0);
	virtual ~BusyWindow();


private:

	QVBoxLayout vbox;
	QProgressBar * bar;
	QLabel * label;
};


#endif