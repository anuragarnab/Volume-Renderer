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

	void resizeGL(int width, int height);
	void initializeGL();
	void paintGL();

	QSize minimumSizeHint() const;
	void loadNewFile(QString filename);

signals:

	public slots :

private:
	bool initTexturesRaw(QString filename);
	bool parseOptions(QString filename);

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

	void map3DTexture(float textureIndex);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

};

#endif // GLWIDGET_H
