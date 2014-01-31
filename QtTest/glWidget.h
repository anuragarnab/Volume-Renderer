#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

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

	void map3DTexture(float textureIndex);

};

#endif // GLWIDGET_H
