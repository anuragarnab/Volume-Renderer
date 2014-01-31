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

	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
	scene->addItem(item);


	// second pane
	for (int row = 0; row < image.height(); ++row){
		for (int col = 0; col < image.width(); ++col){
			QRgb colour = image.pixel(col, row);
			image.setPixel(qGreen(colour), qRed(colour), qBlue(colour));
		}
	}

	QGraphicsPixmapItem* item2 = new QGraphicsPixmapItem(QPixmap::fromImage(image));
	scene2->addItem(item2);

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