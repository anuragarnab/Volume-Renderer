/*
*	renderwindow.h
*	Anurag Arnab
*	23 January 2013
*
*	Window which is the "View" in the Model-View-Controller design pattern
*   Shows the volume rendering as well as the images
*/

#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

//#include <QtGUI>       //Qt4
#include <QtWidgets> //Qt5
#include "glWidget.h"

#include "config.h"

class RenderWindow : public QWidget
{
	Q_OBJECT

public:
	RenderWindow(QWidget *parent = 0);
	virtual ~RenderWindow();

	void addVolumeRenderer(glWidget * volumeRenderer);

	typedef void (RenderWindow::*ProcessFn)(int delta, QImage * image);

private:

	QString filePrefix; // prefix of the image files
	QString extension; // ie jpeg, png etc
	int currentNo; // current image number
	int maxNo;
	int minNo;
	int paddingLength; // ie img001.png has a padding length of 3
	int processOption;
	int processParameter;

	QVBoxLayout mainLayout;
	QHBoxLayout imageLayout;
	QHBoxLayout sliderLayout;

	QSplitter * mainSplit;
	QSplitter * imageSplit;
	QSplitter *sliderSplit;

	QSlider * imageSelector;
	QGraphicsView * imageView;
	QGraphicsScene* scene;
	QGraphicsView * imageView2;
	QGraphicsScene * scene2;
	QLabel * imageNo;
	glWidget * volumeRenderer; 
	QProgressBar * bar;

	QString getFilename(int number);
	void computeTotalImages(void);
	void initialiseSlider(void);

	void processGrayscale(int delta, QImage * image);
	void processBrightness(int delta, QImage * image);
	void processBlur(int kernel, QImage * image);
	void processSaturation(int delta, QImage * image);

	void assignImage(QGraphicsScene * gScene, QImage * gImage);

	void initVolRendererThread(QString filename);

	ProcessFn processingFunctions[N_FILTERS];

public slots:

	bool loadImages();
	void initialiseImages(QString filename);

	void initVolRenderer(QString filename);
	void getProcessOption(int number, int parameter);

	void forceClose(void);

private slots:

	void sliderChanged(int newNumber);

protected:

	void closeEvent(QCloseEvent *event);

signals:

	void closeWindow(void);
};

#endif // RENDERWINDOW_H
