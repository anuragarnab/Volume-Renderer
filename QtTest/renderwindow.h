#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

//#include <QtGUI>       //Qt4
#include <QtWidgets> //Qt5
#include "glWidget.h"

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

	QString getFilename(int number);
	void computeTotalImages(void);
	void initialiseSlider(void);

	void processGrayscale(int delta, QImage * image);
	void processBrightness(int delta, QImage * image);
	void processBlur(int kernel, QImage * image);
	void processSaturation(int delta, QImage * image);

	void assignImage(QGraphicsScene * gScene, QImage * gImage);

	ProcessFn processingFunctions[4];

	public slots:

	bool loadImages();
	void initialiseImages(QString filename);

	void initVolRenderer(QString filename);

	private slots:

	void sliderChanged(int newNumber);
};

#endif // RENDERWINDOW_H
