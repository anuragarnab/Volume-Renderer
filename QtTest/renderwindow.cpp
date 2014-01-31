#include "renderwindow.h"
//#include <QtDebug>
#include <QDebug>

#include "glWidget.h"

// for Qt 4
//#include <QRegExp>

RenderWindow::RenderWindow(QWidget *parent) :
QWidget(parent)
{
	// setting image reading settings
	currentNo = 0;
	minNo = 0;
	maxNo = 0;
	paddingLength = 0;
	volumeRenderer = NULL;

	scene = new QGraphicsScene();
	scene2 = new QGraphicsScene();
	imageSelector = new QSlider(Qt::Horizontal);
	imageView = new QGraphicsView(scene);
	imageView2 = new QGraphicsView(scene2);
	imageNo = new QLabel();

//	imageSelector->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//	imageNo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	sliderSplit = new QSplitter();
	imageSplit = new QSplitter();
	mainSplit = new QSplitter();

//	sliderSplit->addWidget(imageSelector);
//	sliderSplit->addWidget(imageNo);
//	sliderSplit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		sliderLayout.addWidget(imageSelector);
		sliderLayout.addWidget(imageNo);

		//imageLayout.addWidget(imageView);
		//imageLayout.addWidget(imageView2);
	imageSplit->addWidget(imageView);
	imageSplit->addWidget(imageView2);

	/*glWidget * glw = new glWidget(256, 256, 109);

 	mainLayout.addWidget(glw);*/
		mainLayout.addLayout(&sliderLayout);
		//mainLayout.addLayout(&imageLayout);

	//mainSplit->addWidget(sliderSplit);
	mainSplit->addWidget(imageSplit);
	mainSplit->setOrientation(Qt::Vertical);

	mainLayout.addWidget(mainSplit);

	processingFunctions[0] = &RenderWindow::processGrayscale;
	processingFunctions[1] = &RenderWindow::processBrightness;
	processingFunctions[2] = &RenderWindow::processBlur;
	processingFunctions[3] = &RenderWindow::processSaturation;

	//loadImages();
	setLayout(&mainLayout);
}


RenderWindow::~RenderWindow()
{
}

bool RenderWindow::loadImages(void)
{
	/* QImage image;
	image.load("aixz000.JPG");

	if (image.isNull()){
	qDebug() << "No image";
	}

	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
	scene->addItem(item);
	*/

	QImage image;
	image.load(getFilename(currentNo));

	if (image.isNull()){
		qDebug() << "Could not open " << getFilename(currentNo);
		return false;
	}

	scene->clear();
	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
	scene->addItem(item);

	(this->*processingFunctions[3])(35, &image);

	// second pane
	/*for (int row = 0; row < image.height(); ++row){
		for (int col = 0; col < image.width(); ++col){
			QRgb colour = image.pixel(col, row);
			image.setPixel(qGreen(colour), qRed(colour), qBlue(colour));
		}
	}*/


	/*

	Converts to grayscale
	QRgb * line;

	for (int y = 0; y<image.height(); ++y){
		QRgb * line = (QRgb *)image.scanLine(y);

		for (int x = 0; x<image.width(); ++x){
			int average = (qRed(line[x]) + qGreen(line[x]) + qRed(line[x])) / 3;
			image.setPixel(x, y, qRgb(average, average, average));
		}
	}
	*/


	/*
	Darken / lighten 
	QColor oldColor;
	int r, g, b;
	int delta = 35;

	for (int x = 0; x<image.width(); ++x){
		for (int y = 0; y<image.height(); ++y){
			
			oldColor = QColor(image.pixel(x, y));

			r = oldColor.red() + delta;
			g = oldColor.green() + delta;
			b = oldColor.blue() + delta;

			//we check if the new values are between 0 and 255
			r = qBound(0, r, 255);
			g = qBound(0, g, 255);
			b = qBound(0, b, 255);

			image.setPixel(x, y, qRgb(r, g, b));
		}
	}
	*/

	/*
	Blur

	int kernel[5][5] = { { 0, 0, 1, 0, 0 },
	{ 0, 1, 3, 1, 0 },
	{ 1, 3, 7, 3, 1 },
	{ 0, 1, 3, 1, 0 },
	{ 0, 0, 1, 0, 0 } };
	int kernelSize = 5;
	int sumKernel = 27;
	int r, g, b;
	QColor color;

	for (int x = kernelSize / 2; x< image.width() - (kernelSize / 2); x++){
		for (int y = kernelSize / 2; y< image.height() - (kernelSize / 2); y++){

			r = 0;
			g = 0;
			b = 0;

			for (int i = -kernelSize / 2; i <= kernelSize / 2; i++){
				for (int j = -kernelSize / 2; j <= kernelSize / 2; j++){
					color = QColor(image.pixel(x + i, y + j));
					r += color.red()*kernel[kernelSize / 2 + i][kernelSize / 2 + j];
					g += color.green()*kernel[kernelSize / 2 + i][kernelSize / 2 + j];
					b += color.blue()*kernel[kernelSize / 2 + i][kernelSize / 2 + j];
				}
			}

			r = qBound(0, r / sumKernel, 255);
			g = qBound(0, g / sumKernel, 255);
			b = qBound(0, b / sumKernel, 255);

			image.setPixel(x, y, qRgb(r, g, b));

		}
	}
	*/
	/*
	QColor oldColor;
	QColor newColor;
	int h, s, l;
	int delta = 35;

	for (int x = 0; x< image.width(); x++){
		for (int y = 0; y<image.height(); y++){
			oldColor = QColor(image.pixel(x, y));

			newColor = oldColor.toHsl();
			h = newColor.hue();
			s = newColor.saturation() + delta;
			l = newColor.lightness();

			//we check if the new value is between 0 and 255
			s = qBound(0, s, 255);

			newColor.setHsl(h, s, l);

			image.setPixel(x, y, qRgb(newColor.red(), newColor.green(), newColor.blue()));
		}
	}
	*/


	return true;
}

void RenderWindow::initialiseImages(QString filename)
{

	QString digits;
	QString temp = filename;

	//qDebug() << "Entered slot\n" << filename;

	/* int index = filename.lastIndexOf("/");
	if (index >= 0)
	{
	filename = filename.remove(0, index+1).trimmed();
	}*/


	int extensionIndex = temp.lastIndexOf(".");
	if (extensionIndex >= 0)
	{
		extension = temp.remove(0, extensionIndex + 1).trimmed();
	}
	qDebug() << filename;


	// This is for Qt4

	/*QRegExp rx;
	rx.setPattern("(.*?)([\\d]+)\\..*");
	if (rx.indexIn(filename) != -1){
	filePrefix = rx.cap(1); // "image
	digits = rx.cap(2); // "001
	qDebug() << digits;
	qDebug() << filePrefix;
	qDebug() << extension;

	minNo = digits.toInt();
	currentNo = minNo;
	maxNo = minNo;
	paddingLength = digits.length();

	qDebug() << getFilename(currentNo);
	qDebug() << getFilename(9);
	qDebug() << getFilename(99);
	qDebug() << getFilename(75);

	computeTotalImages();
	initialiseSlider();
	}*/


	QRegularExpression re("(.*?)([\\d]+)\\..*");
	QRegularExpressionMatch match = re.match(filename);
	if (match.hasMatch()) {
		filePrefix = match.captured(1); // "image
		digits = match.captured(2); // "001
		qDebug() << digits;
		qDebug() << filePrefix;
		qDebug() << extension;

		minNo = digits.toInt();
		currentNo = minNo;
		maxNo = minNo;
		paddingLength = digits.length();

		qDebug() << getFilename(currentNo);
		qDebug() << getFilename(9);
		qDebug() << getFilename(99);
		qDebug() << getFilename(75);

		computeTotalImages();
		initialiseSlider();
	}


}

QString RenderWindow::getFilename(int number)
{
	QString result = "";
	for (int i = 0; i < paddingLength - 1; ++i){
		result.append("0");
	}
	result = result + QString::number(number);
	return filePrefix + result.remove(0, result.length() - paddingLength) + "." + extension;
}

void RenderWindow::computeTotalImages(void)
{
	maxNo = minNo;
	QFile test(getFilename(minNo));

	while (test.exists()){
		test.close();
		test.setFileName(getFilename(++maxNo));
	}

	--maxNo;
	qDebug() << "Max number " << maxNo;
}

void RenderWindow::initialiseSlider(void)
{
	imageSelector->setMinimum(minNo);
	imageSelector->setMaximum(maxNo);

	currentNo = minNo;
	imageSelector->setValue(currentNo);

	connect(imageSelector, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
	emit imageSelector->valueChanged(imageSelector->value());
	//sliderChanged(0);// Above line creates a problem in Qt 4.8
}

void RenderWindow::sliderChanged(int newNumber)
{
	qDebug() << newNumber;
	currentNo = newNumber;
	if (loadImages()){
		imageNo->setText(QString::number(currentNo) + "/" + QString::number(maxNo));
	}
}

void RenderWindow::addVolumeRenderer(glWidget * volumeRenderer)
{
	mainLayout.addWidget(volumeRenderer);
}

void RenderWindow::initVolRenderer(QString filename)
{
	if (volumeRenderer == NULL){
		//volumeRenderer = new glWidget(256, 256, 109);
		volumeRenderer = new glWidget(filename);
			//mainLayout.addWidget(volumeRenderer);
		mainSplit->addWidget(volumeRenderer);
	}
	else{
		//volumeRenderer = new glWidget(256, 256, 100);
		/*
		QLayoutItem *child;
		child = mainLayout.takeAt(2);

		delete child->widget();
		delete child;

		volumeRenderer = new glWidget(filename);
			//mainLayout.addWidget(volumeRenderer);
		mainSplit->addWidget(volumeRenderer);
		*/

		volumeRenderer->loadNewFile(filename);
	}
	qDebug() << "Got " << filename;
}

void RenderWindow::processGrayscale(int delta, QImage * image)
{
	QRgb * line;

	for (int y = 0; y<image->height(); ++y){
		QRgb * line = (QRgb *)image->scanLine(y);

		for (int x = 0; x<image->width(); ++x){
			int average = (qRed(line[x]) + qGreen(line[x]) + qRed(line[x])) / 3;
			image->setPixel(x, y, qRgb(average, average, average));
		}
	}

	assignImage(scene2, image);
}

void RenderWindow::processBrightness(int delta, QImage * image)
{
	QColor oldColor;
	int r, g, b;

	for (int x = 0; x<image->width(); ++x){
		for (int y = 0; y<image->height(); ++y){

			oldColor = QColor(image->pixel(x, y));

			r = oldColor.red() + delta;
			g = oldColor.green() + delta;
			b = oldColor.blue() + delta;

			//we check if the new values are between 0 and 255
			r = qBound(0, r, 255);
			g = qBound(0, g, 255);
			b = qBound(0, b, 255);

			image->setPixel(x, y, qRgb(r, g, b));
		}
	}

	assignImage(scene2, image);
}

void RenderWindow::processBlur(int kernel, QImage * image)
{
	int matrix[5][5] = 
	{ { 0, 0, kernel/4, 0, 0 },
	{ 0, kernel/4, kernel/2, kernel/4, 0 },
	{ kernel/4, kernel/2, kernel, kernel/2, kernel/4 },
	{ 0, kernel/4, kernel/2, kernel/4, 0 },
	{ 0, 0, kernel/4 , 0, 0 } };
	
	int kernelSize = 5;
	//int sumKernel = kernel/4 + kernel/4 + kernel/2 + kernel/4 + kernel/4 + kernel/2 + kernel + kernel/2 + kernel/4 + kernel/4 + kernel/2 + kernel/4 + kernel/4;
	
	int sumKernel = 0;
	for (int row = 0; row < 5; ++row){
		for (int col = 0; col < 5; ++col){
			sumKernel += matrix[row][col];
		}
	}
	
	qDebug() << "Sum kernel " << sumKernel;

	int r, g, b;
	QColor color;

	for (int x = kernelSize / 2; x< image->width() - (kernelSize / 2); x++){
		for (int y = kernelSize / 2; y< image->height() - (kernelSize / 2); y++){

			r = 0;
			g = 0;
			b = 0;

			for (int i = -kernelSize / 2; i <= kernelSize / 2; i++){
				for (int j = -kernelSize / 2; j <= kernelSize / 2; j++){
					color = QColor(image->pixel(x + i, y + j));
					r += color.red()*matrix[kernelSize / 2 + i][kernelSize / 2 + j];
					g += color.green()*matrix[kernelSize / 2 + i][kernelSize / 2 + j];
					b += color.blue()*matrix[kernelSize / 2 + i][kernelSize / 2 + j];
				}
			}

			r = qBound(0, r / sumKernel, 255);
			g = qBound(0, g / sumKernel, 255);
			b = qBound(0, b / sumKernel, 255);

			image->setPixel(x, y, qRgb(r, g, b));

		}
	}

	assignImage(scene2, image);
}

void RenderWindow::processSaturation(int delta, QImage * image)
{
	QColor oldColor;
	QColor newColor;
	int h, s, l;

	for (int x = 0; x< image->width(); x++){
		for (int y = 0; y<image->height(); y++){
			oldColor = QColor(image->pixel(x, y));

			newColor = oldColor.toHsl();
			h = newColor.hue();
			s = newColor.saturation() + delta;
			l = newColor.lightness();

			//we check if the new value is between 0 and 255
			s = qBound(0, s, 255);

			newColor.setHsl(h, s, l);

			image->setPixel(x, y, qRgb(newColor.red(), newColor.green(), newColor.blue()));
		}
	}

	assignImage(scene2, image);
}


void RenderWindow::assignImage(QGraphicsScene * gScene, QImage * gImage)
{
	gScene->clear();
	QGraphicsPixmapItem* item2 = new QGraphicsPixmapItem(QPixmap::fromImage(*gImage));
	gScene->addItem(item2);
}
