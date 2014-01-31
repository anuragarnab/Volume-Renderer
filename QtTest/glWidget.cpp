/*
*	glWidget.cpp
*	Anurag Arnab
*	23 January 2013
*
*	Implementation of glWidget - the OpenGL widget that performs volume rendering
*   Subclasses QGLWidget
*/
#include "glwidget.h"
#include <qopenglext.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>

//#include <QCoreApplication>
/*
*
* Constructor. Used for debugging with default arguements
*
*/
glWidget::glWidget(int width, int height, int count, QWidget *parent) :
QGLWidget(parent)
{
	IMAGEWIDTH = width;
	IMAGEHEIGHT = height;
	IMAGECOUNT = count;
	ALPHA_SCALE = 1.0f;
	SAMPLE_STEP = 0.003f;
	ALPHA_THRESHOLD = 0.07f;
	DATAFILE = "head.raw";

	loadSuccess = false;
}

/*
*
* Constructor. Only takes in parameter of the config file
*
*/
glWidget::glWidget(QString filename, QWidget *parent) :
QGLWidget(parent), currentFilename(filename)
{
	loadSuccess = false;
	parseOptions(filename);
	
}

/*
Constructor. Loads in texture from a sequence of images
*/
glWidget::glWidget(QString prefix, QString _extension, int _paddingLength, int _minNo, int _maxNo, float alphaThreshold, QWidget * parent) :
QGLWidget(parent), filePrefix(prefix), extension(_extension), paddingLength(_paddingLength), minNo(_minNo), maxNo(_maxNo)
{
	ALPHA_SCALE = 1.0f;
	SAMPLE_STEP = 0.003f;
	ALPHA_THRESHOLD = alphaThreshold;
	DATAFILE = "head.raw";

	QImage image;
	image.load(getFilename(minNo));

	if (image.isNull()){
		qDebug() << "Could not open first image";
	}

	IMAGEWIDTH = image.width();
	IMAGEHEIGHT = image.height();
	IMAGECOUNT = maxNo - minNo + 1;

	qDebug() << IMAGECOUNT;

	loadSuccess = false;
}

/*
*
* Called when the widget is resized
* Maintains the aspect ratio and sets up the orthographic projection
*
*/
void glWidget::resizeGL(int width, int height){
	
	GLfloat dOrthoSize = 1.0f;
	
	GLdouble aspectRatio = (GLdouble)(width) / (GLdouble)(height);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (width <= height)
	{
		glOrtho(-dOrthoSize, dOrthoSize, -(dOrthoSize / aspectRatio),
			dOrthoSize / aspectRatio, 2.0f*-dOrthoSize, 2.0f*dOrthoSize);
	}
	else
	{
		glOrtho(-dOrthoSize * aspectRatio, dOrthoSize * aspectRatio,
			-dOrthoSize, dOrthoSize, 2.0f*-dOrthoSize, 2.0f*dOrthoSize);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
*
* Called to initialise OpenGL functions
* Required when subclassing QGLWidget
*
*/
void glWidget::initializeGL(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	if (/*!initTexturesRaw(DATAFILE)*/ !initTexturesFiles()){
		qDebug() << "Could not load texture";
	}
	else{
		lastGoodFilename = currentFilename;
		loadSuccess = true;
	}
}

/*
*
* Rendering function
*
*/
void glWidget::paintGL(){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (loadSuccess){

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, ALPHA_THRESHOLD);

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		// Translate and make 0.5f as the center 
		// (texture co ordinate is from 0 to 1.
		// so center of rotation has to be 0.5f)
		glTranslatef(0.5f, 0.5f, 0.5f);

		// A scaling applied to normalize the axis 
		// (Usually the number of slices will be less so if this is not - 
		// normalized then the z axis will look bulky)
		// Flipping of the y axis is done by giving a negative value in y axis.
		// This can be achieved either by changing the y co ordinates in -
		// texture mapping or by negative scaling of y axis

		glScaled((float)IMAGEWIDTH / (float)IMAGEWIDTH,
			-1.0f*(float)IMAGEWIDTH / (float)(float)IMAGEHEIGHT,
			(float)IMAGEWIDTH / (float)IMAGECOUNT);

		/*if (IMAGECOUNT > IMAGEWIDTH)
		{
		glScaled((float)IMAGECOUNT / (float)IMAGEWIDTH,
		-1.0f*(float)IMAGECOUNT / (float)(float)IMAGEHEIGHT,
		(float)IMAGECOUNT / (float)IMAGECOUNT);
		}*/

		// Apply the user provided transformations
		glMultMatrixd(transManager.GetMatrix());

		//glRotatef(25.0f, 0.0, 1.0, 0.0);

		glTranslatef(-0.5f, -0.5f, -0.5f);

		glEnable(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, textureID3D);


		for (float fIndx = -1.0f; fIndx <= 1.0f; fIndx += SAMPLE_STEP)
		{
			glBegin(GL_QUADS);
			map3DTexture(fIndx);
			glEnd();
		}
	}
}

/*
*
* Creates a 3D texture from the raw file specified by "filename"
*
*/
bool glWidget::initTexturesRaw(QString filename)
{

	float factor = 1.0f;
	float gradient = (1.0f - ALPHA_SCALE) / (IMAGECOUNT - 1);

	FILE *file;
	file = fopen(filename.toStdString().c_str(), "rb");

	if (file == NULL)
	{
		fprintf(stderr, "Could not open texture file\n");
		return false;
	}

	// Holds the luminance buffer
	char* chRGBABuffer = new char[IMAGEWIDTH * IMAGEHEIGHT * IMAGECOUNT * 4];
	char* chBuffer = new char[IMAGEHEIGHT * IMAGEWIDTH * IMAGECOUNT];

	//Only create 1 3D texture now
	glGenTextures(1, (GLuint*)&textureID3D);

	// Read each frames and construct the texture
	fread(chBuffer, sizeof(unsigned char), IMAGEHEIGHT*IMAGEWIDTH*IMAGECOUNT, file);

	// Set the properties of the texture.
	glBindTexture(GL_TEXTURE_3D, textureID3D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Convert the data to RGBA data.
	// Here we are simply putting the same value to R, G, B and A channels.
	// This can be changed depending on the source data
	// Usually for raw data, the alpha value will
	// be constructed by a threshold value given by the user
	for (int nIndx = 0; nIndx < IMAGEWIDTH * IMAGEHEIGHT * IMAGECOUNT; ++nIndx)
	{
		chRGBABuffer[nIndx * 4] = chBuffer[nIndx];
		chRGBABuffer[nIndx * 4 + 1] = chBuffer[nIndx];
		chRGBABuffer[nIndx * 4 + 2] = chBuffer[nIndx];
		chRGBABuffer[nIndx * 4 + 3] = (char)(chBuffer[nIndx] * factor);

		if ((nIndx % (IMAGEWIDTH * IMAGEHEIGHT) == 0) && nIndx != 0)
		{
			factor = factor - gradient;
		}
	}

	PFNGLTEXIMAGE3DPROC glTexImage3D;
	glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, IMAGEWIDTH, IMAGEHEIGHT, IMAGECOUNT, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)chRGBABuffer);

	glBindTexture(GL_TEXTURE_3D, 0);

	int err = glGetError();
	if (err > 0)
	{
		printf("Error %d\n", err);
	}
	fclose(file);

	delete[] chRGBABuffer;
	delete[] chBuffer;
	return true;
}

/*
*
* Maps 3D texture for rendering
*
*/
void glWidget::map3DTexture(float textureIndex)
{

	GLfloat dViewPortSize = 1.0f;

	glTexCoord3f(0.0f, 0.0f, ((float)textureIndex + 1.0f) / 2.0f);
	glVertex3f(-dViewPortSize, -dViewPortSize, textureIndex);  // bottom left corner + z index
	glTexCoord3f(1.0f, 0.0f, ((float)textureIndex + 1.0f) / 2.0f);
	glVertex3f(dViewPortSize, -dViewPortSize, textureIndex); // top left corner + z index
	glTexCoord3f(1.0f, 1.0f, ((float)textureIndex + 1.0f) / 2.0f);
	glVertex3f(dViewPortSize, dViewPortSize, textureIndex);  // top right corner + z index
	glTexCoord3f(0.0f, 1.0f, ((float)textureIndex + 1.0f) / 2.0f);
	glVertex3f(-dViewPortSize, dViewPortSize, textureIndex); // bottom right corner + z index
}

/*
*
* Size hint ensures that Widget is created with an appropriate size
* Without this, it is created to be very small, and is not visible even when resizing the parent window
* It is not possible to assign variables to the width and height
*
*/
QSize glWidget::minimumSizeHint() const
{
	return QSize(256, 256);
	//return QSize(IMAGEWIDTH, IMAGEHEIGHT);
}

/*
*
* Parse the config file to set up the state of the renderer
*
*/
bool glWidget::parseOptions(QString filename)
{

	int count = 0;
	int index = -1;

	QFile inputFile(filename);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			QString line = in.readLine();
			
			if (line.length() != 0 && !line.startsWith("#")){

				++count; 

				switch (count)
				{
				case(pIMAGEWIDTH) :
					IMAGEWIDTH = line.toInt();
					qDebug() << "Image width: " << IMAGEWIDTH;
					break;
				case(pIMAGEHEIGHT) :
					IMAGEHEIGHT = line.toInt();
					qDebug() << "Image height: " << IMAGEHEIGHT;
					break;
				case(pIMAGECOUNT) :
					IMAGECOUNT = line.toInt();
					qDebug() << "Image count: " << IMAGECOUNT;
					break;
				case(pALPHA_THRESHOLD) :
					ALPHA_THRESHOLD = line.toFloat();
					qDebug() << "Alpha threshold: " << ALPHA_THRESHOLD;
					break;
				case(pSAMPLE_STEP) :
					SAMPLE_STEP = line.toFloat();
					qDebug() << "Sample step: " << SAMPLE_STEP;
					break;
				case(pDATAFILE) :
					DATAFILE = line;

					index = filename.lastIndexOf("/");
					if (index >= 0)
					{
						qDebug() << filename;
						filename = filename.remove(index + 1, filename.length()); // remove function stops at end if you specify more than it can remove
						//filename = filename.remove(0, index+1).trimmed();
						qDebug() << filename;
						DATAFILE = filename + DATAFILE;
					}

					qDebug() << "Data file: " << DATAFILE;
					break;
				case(pALPHA_SCALE) :
					ALPHA_SCALE = line.toFloat();
					qDebug() << "Alpha scaling: " << ALPHA_SCALE;
					break;
				
				default:
					break;
				}
			}
		}
		inputFile.close();
	}

	if (count >= pLAST)
	{
		return true;
	}
	return false;
}

/*
*
* Captures postion of mouse on press
*
*/
void glWidget::mousePressEvent(QMouseEvent *event)
{
	lastPosition = event->pos();
}

/*
*
* Triggered when mouse is dragged
* Used to control rotation of the volume
*
*/
void glWidget::mouseMoveEvent(QMouseEvent *event)
{

	int x = event->x();
	int y = event->y();

	if (((x - lastPosition.x() != 0)) || ((y - lastPosition.y()) != 0))
	{
		transManager.Rotate((float)(lastPosition.y() - y), (float)(lastPosition.x() - x), 0.0f);
		lastPosition = event->pos();
	}

	updateGL();
}

/*
*
* Loads a new data file for rendering
* In case the new configuration file is bad, the previous one is used
*
*/
void glWidget::loadNewFile(QString filename)
{
	parseOptions(filename);
	if (!initTexturesRaw(DATAFILE)){
		qDebug() << "Could not load texture";

		parseOptions(lastGoodFilename);
		if (initTexturesRaw(DATAFILE)){
			loadSuccess = true;
		}
	}
	else{
		currentFilename = filename;
		lastGoodFilename = filename;
		loadSuccess = true;
	}
	updateGL();
}


void glWidget::loadNewFile(QString prefix, QString _extension, int _paddingLength, int _minNo, int _maxNo)
{

	filePrefix = prefix;
	extension = _extension;
	paddingLength = _paddingLength;
	minNo = _minNo;
	maxNo = _maxNo;

	QImage image;
	image.load(getFilename(minNo));

	if (image.isNull()){
		qDebug() << "Could not open first image";
	}

	IMAGEWIDTH = image.width();
	IMAGEHEIGHT = image.height();
	IMAGECOUNT = maxNo - minNo + 1;

	qDebug() << "Width: " << IMAGEWIDTH << "Height: " << IMAGEHEIGHT << " Count: " << IMAGECOUNT;


	loadSuccess = initTexturesFiles();

	if (!loadSuccess){
		qDebug() << "Could not load texture";
	}

	updateGL();
}


bool glWidget::initTexturesFiles(void)
{

	float factor = 1.0f;
	float gradient = (1.0f - ALPHA_SCALE) / (IMAGECOUNT - 1);

	// Holds the luminance buffer
	char* chRGBABuffer = new char[IMAGEWIDTH * IMAGEHEIGHT * IMAGECOUNT * 4];

	//Only create 1 3D texture now
	glGenTextures(1, (GLuint*)&textureID3D);

	int count = 0;

	// Read each frames and construct the texture
	for (int i = minNo; i <= maxNo; ++i){

		QImage image;
		image.load(getFilename(i));

		if (image.isNull()){
			qDebug() << "Could not open " << getFilename(i);
			return false;
		}

		for (int y = 0; y < image.height(); ++y){
			QRgb * line = (QRgb *)image.scanLine(y);

			for (int x = 0; x < image.width(); ++x){
				char average = (char) ( (qRed(line[x]) + qGreen(line[x]) + qRed(line[x])) / 3 );
				chRGBABuffer[count * 4] = average;
				chRGBABuffer[count * 4 + 1] = average;
				chRGBABuffer[count * 4 + 2] = average;
				chRGBABuffer[count * 4 + 3] = (char)(average * factor);

				if ((count % (IMAGEWIDTH * IMAGEHEIGHT) == 0) && count != 0)
				{
					factor = factor - gradient;
				}

				++count;
			}
		}

		//QCoreApplication::processEvents();

	}


	// Set the properties of the texture.
	glBindTexture(GL_TEXTURE_3D, textureID3D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//To do: #IFDEF WINDOWS
	PFNGLTEXIMAGE3DPROC glTexImage3D;
	glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, IMAGEWIDTH, IMAGEHEIGHT, IMAGECOUNT, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)chRGBABuffer);

	glBindTexture(GL_TEXTURE_3D, 0);

	int err = glGetError();
	if (err > 0)
	{
		printf("OpenGL Error %d\n", err);
	}

	delete[] chRGBABuffer;
	return true;
}

QString glWidget::getFilename(int number)
{
	QString result = "";
	for (int i = 0; i < paddingLength - 1; ++i){
		result.append("0");
	}
	result = result + QString::number(number);
	return filePrefix + result.remove(0, result.length() - paddingLength) + "." + extension;
}

void glWidget::setAlphaThresh(float alphaThresh)
{
	ALPHA_THRESHOLD = alphaThresh;
	updateGL();
}

void glWidget::setAlphaScale(float alphaScale)
{
	ALPHA_SCALE = alphaScale;

	loadSuccess = initTexturesFiles();

	if (!loadSuccess){
		qDebug() << "Could not load texture";
	}

	updateGL();
}