#include "glwidget.h"
#include <qopenglext.h>

glWidget::glWidget(int width, int height, int count, QWidget *parent) :
QGLWidget(parent)
{
	IMAGEWIDTH = width;
	IMAGEHEIGHT = height;
	IMAGECOUNT = count;
}

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

void glWidget::initializeGL(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	if (!initTexturesRaw("head.raw")){
		//qDebug() << "Could not load texture";
	}
}

void glWidget::paintGL(){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.07f);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	// A scaling applied to normalize the axis 
	// (Usually the number of slices will be less so if this is not - 
	// normalized then the z axis will look bulky)
	// Flipping of the y axis is done by giving a negative value in y axis.
	// This can be achieved either by changing the y co ordinates in -
	// texture mapping or by negative scaling of y axis

	glScaled((float)IMAGEWIDTH / (float)IMAGEWIDTH,
		1.0f*(float)IMAGEWIDTH / (float)(float)IMAGEHEIGHT,
		(float)IMAGEWIDTH / (float)IMAGECOUNT);

	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, textureID3D);


	for (float fIndx = -1.0f; fIndx <= 1.0f; fIndx += 0.003f)
	{
		glBegin(GL_QUADS);
		map3DTexture(fIndx);
		glEnd();
	}
}

bool glWidget::initTexturesRaw(QString filename)
{

	float ALPHA_SCALE = 1.0f;

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