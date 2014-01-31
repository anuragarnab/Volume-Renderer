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
#include "xmlparser.h"

#include "config.h"

class ControlWindow : public QWidget
{
	Q_OBJECT

public:
	ControlWindow(QWidget *parent = 0);
	~ControlWindow();

	QLineEdit * getLineThreshold(void);
	int getNumberFilters(void);

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
	//QPushButton * volumeButton;

	QRadioButton ** radioButtons;
//	QLineEdit ** filterOptions;
	QGridLayout ** renderGrid;

	void initialiseRenderOptions(void);
	void initialiseVolumeOptions(void);
	void toggleButtons(int option);
	void minimiseButtons(void);
	void setRenderVisibility(bool isVisible, int i);

	int renderButtonHits;

	int uploadHits;
	int volRenderHits;
	int volRenderClicks;

	bool textChanged;
	bool textFinished;

	xmlParser xml;

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

	void lineChanged(QString line);
	void lineFinished(void);

signals:
	void imageFilename(QString filname);
	void volRendConfigFile(QString filename);
	void imageFilterChosen(int number, QString parameters);
	void alphaScale(QString line);

	void closeWindow(void);


};

#endif // CONTROLWINDOW_H
