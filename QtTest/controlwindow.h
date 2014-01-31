/*
*	controlwindow.h
*	Anurag Arnab
*	23 January 2013
*
*	Header for Control Window - the "Controller" in the Model-View-Control (MVC) design pattern
*/

#ifndef CONTROLWINDOW_H
#define CONTROLWINDOW_H


#include <QMainWindow>
//#include <QtGui> // Qt4
#include <QtWidgets> //Qt5

#include "config.h"

class ControlWindow : public QWidget
{
	Q_OBJECT

public:
	ControlWindow(QWidget *parent = 0);
	~ControlWindow();

private:

	QCommandLinkButton * uploadOCT;
	QCommandLinkButton * renderFilter;
	QCommandLinkButton * volumeRendering;
	QCommandLinkButton * computeFingerprint;
	QCommandLinkButton * enhanceFingerprint;

	QGroupBox * groupRenderFilter;

	QVBoxLayout mainLayout;
	QVBoxLayout renderLayout;
	QVBoxLayout volumeLayout;

	QGroupBox * groupVolume;
	QLineEdit * lineAlphaThresh;
	QLineEdit * lineAlphaScale;
	QLabel * label1;
	QLabel * label2;
	QPushButton * volumeButton;

	QRadioButton ** radioButtons;
	QLineEdit ** filterOptions;

	void initialiseRenderOptions(void);
	void initialiseVolumeOptions(void);
	void toggleButtons(int option);
	void minimiseButtons(void);

	int renderButtonHits;

	int uploadHits;
	int volRenderHits;
	int volRenderClicks;

protected:

	void closeEvent(QCloseEvent *event);

public slots:

	void forceClose(void);

private slots:
	void handleVolumeRendering(void);
	void handleRenderFilter(void);
	void handleUploadOCT(void);
	void handleComputeFingerprint(void);
	void handleEnhanceFingerprint(void);

	void radioButtonToggled(void);

signals:
	void imageFilename(QString filname);
	void volRendConfigFile(QString filename);
	void imageFilterChosen(int number, int paramter);

	void closeWindow(void);

};

#endif // CONTROLWINDOW_H
