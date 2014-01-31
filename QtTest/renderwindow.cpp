#include "renderwindow.h"
#include <QDebug>

#include "glWidget.h"

RenderWindow::RenderWindow(QWidget *parent) :
QWidget(parent)
{
	// setting image reading settings
	currentNo = 0;
	minNo = 0;
	maxNo = 0;
	paddingLength = 0;
	volumeRenderer = NULL;
	processOption = -1;

	scene = new QGraphicsScene();
	scene2 = new QGraphicsScene();
	imageSelector = new QSlider(Qt::Horizontal);
	imageView = new QGraphicsView(scene);
	imageView2 = new QGraphicsView(scene2);
	imageNo = new QLabel();

	sliderSplit = new QSplitter();
	imageSplit = new QSplitter();
	mainSplit = new QSplitter();

	sliderLayout.addWidget(imageSelector);
	sliderLayout.addWidget(imageNo);

	imageSplit->addWidget(imageView);
	imageSplit->addWidget(imageView2);

	mainLayout.addLayout(&sliderLayout);
	mainSplit->addWidget(imageSplit);
	mainSplit->setOrientation(Qt::Vertical);

	mainLayout.addWidget(mainSplit);

	processingFunctions[0] = &RenderWindow::processGrayscale;
	processingFunctions[1] = &RenderWindow::processBrightness;
	processingFunctions[2] = &RenderWindow::processBlur;
	processingFunctions[3] = &RenderWindow::processSaturation;

	//loadImages();
	setLayout(&mainLayout);
	setWindowTitle("Rendering window");
	setMinimumSize(400, 300);
}


RenderWindow::~RenderWindow()
{
}

bool RenderWindow::loadImages(void)
{

	QImage image;
	image.load(getFilename(currentNo));

	if (image.isNull()){
		qDebug() << "Could not open " << getFilename(currentNo);
		return false;
	}

	scene->clear();
	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
	scene->addItem(item);

	qDebug() << "processOption = " << processOption;
	if (processOption > -1){
		(this->*processingFunctions[processOption])(processParameter, &image);
	}

	return true;
}

void RenderWindow::initialiseImages(QString filename)
{

	QString digits;
	QString temp = filename;

	int extensionIndex = temp.lastIndexOf(".");
	if (extensionIndex >= 0)
	{
		extension = temp.remove(0, extensionIndex + 1).trimmed();
	}
	qDebug() << filename;

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
		volumeRenderer = new glWidget(filename);
		mainSplit->addWidget(volumeRenderer);
	}
	else{
		volumeRenderer->loadNewFile(filename);
	}
}

void RenderWindow::getProcessOption(int number, int parameter)
{
	processOption = number;
	processParameter = parameter;
	loadImages();
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

			//check if the new values are between 0 and 255
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

	if (kernel == 0)
	{
		assignImage(scene2, image);
		return;
	}

	int matrix[5][5] = 
	{ { 0, 0, kernel/4, 0, 0 },
	{ 0, kernel/4, kernel/2, kernel/4, 0 },
	{ kernel/4, kernel/2, kernel, kernel/2, kernel/4 },
	{ 0, kernel/4, kernel/2, kernel/4, 0 },
	{ 0, 0, kernel/4 , 0, 0 } };
	
	int kernelSize = 5;
	
	int sumKernel = 0;
	for (int row = 0; row < 5; ++row){
		for (int col = 0; col < 5; ++col){
			sumKernel += matrix[row][col];
		}
	}
	
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

			//check if the new value is between 0 and 255
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

void RenderWindow::forceClose(void)
{
	close();
}

void RenderWindow::closeEvent(QCloseEvent *event)
{
	emit closeWindow();
	event->accept();
}