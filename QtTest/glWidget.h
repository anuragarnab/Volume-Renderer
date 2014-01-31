#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

class glWidget : public QGLWidget
{
	Q_OBJECT
public:
	explicit glWidget(int width, int height, int count, QWidget *parent = 0);

	void resizeGL(int width, int height);
	void initializeGL();
	void paintGL();

signals:

	public slots :

private:
	bool glWidget::initTexturesRaw(QString filename);

	float ALPHA_SCALE;
	int IMAGEWIDTH;
	int IMAGEHEIGHT;
	int IMAGECOUNT;
	int textureID3D;

	void map3DTexture(float textureIndex);

};

#endif // GLWIDGET_H
