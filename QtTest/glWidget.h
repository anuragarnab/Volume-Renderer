/*
*	glWidget.h
*	Anurag Arnab
*	23 January 2013
*
*	Header of glWidget - the OpenGL widget that performs volume rendering
*   Subclasses QGLWidget
*/

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QKeyEvent>
#include "TransformationManager.h"

#define pIMAGEWIDTH 1
#define pIMAGEHEIGHT 2
#define pIMAGECOUNT 3
#define pALPHA_THRESHOLD 4
#define pSAMPLE_STEP 5
#define pDATAFILE 6
#define pALPHA_SCALE 7
#define pLAST pALPHA_SCALE

class glWidget : public QGLWidget
{
	Q_OBJECT
public:
	explicit glWidget(int width, int height, int count, QWidget *parent = 0);
	explicit glWidget(QString filename, QWidget * parent = 0);
	explicit glWidget(QString prefix, QString _extension, int paddingLength, int minNo, int maxNo, float alphaThreshold = 0.07f, QWidget * parent = 0);

	void resizeGL(int width, int height);
	void initializeGL();
	void paintGL();

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	void loadNewFile(QString filename);
	void loadNewFile(QString prefix, QString _extension, int _paddingLength, int _minNo, int _maxNo);

	void setAlphaThresh(float alphaThresh);
	void setAlphaScale(float alphaScale);

	void displayBlack(void);


signals:

	public slots :

private:
	bool initTexturesRaw(QString filename);
	bool initTexturesFiles(void);
	bool parseOptions(QString filename);

	bool loadSuccess;

	float ALPHA_SCALE;
	int IMAGEWIDTH;
	int IMAGEHEIGHT;
	int IMAGECOUNT;
	float SAMPLE_STEP;
	float ALPHA_THRESHOLD;
	QString DATAFILE;
	int textureID3D;
	QPoint lastPosition;
	TransformationManager transManager;

	QString lastGoodFilename;
	QString currentFilename;

	QString filePrefix;
	QString extension;
	int paddingLength;
	int maxNo;
	int minNo;

	void map3DTexture(float textureIndex);
	QString getFilename(int number);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

};

#endif // GLWIDGET_H
