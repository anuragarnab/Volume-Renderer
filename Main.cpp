/*
Main.cpp
Anurag Arnab - 10 January 2014

Driver for Volume Renderer
First reads in configuration data from the specified file (config.txt by default)
Then sets up GLUT and creates textures for rendering by OpenGL
The display can be rotated by keyboard or mouse, callbacks provided by GLUT handle this
*/

#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <stdio.h>
#include "Main.h"
#include "Config.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "TransformationManager.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

TransformationManager transManager; 

int main(int argc, char** argv)
{
	if (!parseOptions(CONFIG_FILE))
	{
		fprintf(stderr, "Could not parse configuration file\n");
		return 1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(IMAGEWIDTH, IMAGEHEIGHT);

	// Apparently this can prevent the program from crashing in Ubuntu in VirtualBox
	if (!glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
	{
		fprintf(stdout, "Unable to create window.");
		exit(1);
	}
	glutCreateWindow(WINDOW_TITLE.c_str());
	
	glutDisplayFunc(&render);
	glutReshapeFunc(&resize);
	glutKeyboardFunc(&keyboard);
	glutMotionFunc(&mouseDrag);
	glutMouseFunc(&mouseClick);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS); // so can do processing when window is closed

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glewInit();

	fprintf(stdout, "OpenGL Version: %s\n", glGetString(GL_VERSION));

	bool loadSuccess = false;

	if (RAW_OR_IMAGES == 0){
		loadSuccess = initTexturesRaw(DATAFILE);
	}
	else{
		loadSuccess = initTexturesFiles(DATAFILE);
	}

	if (!loadSuccess)
	{
		fprintf(stderr, "Failed to initialise textures\nCheck DATAFILE in config file");
	}

	glutMainLoop();
	fprintf(stdout, "Window closed\n");
	return 0;
}

/*
Render image for display
*/
void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA
		);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, ALPHA_THRESHOLD);


	if (ORTHO_OR_PERSP == 0)
	{
		glLoadIdentity();             // clear the matrix 
		gluLookAt(EYE_X, EYE_Y, EYE_Z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 	// viewing transformation  
	}

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
		1.0f*(float)IMAGEWIDTH / (float)(float)IMAGEHEIGHT,
		(float)IMAGEWIDTH / (float)IMAGECOUNT);

	/*if (IMAGECOUNT > IMAGEWIDTH)
	{
		glScaled((float)IMAGECOUNT / (float)IMAGEWIDTH,
			-1.0f*(float)IMAGECOUNT / (float)(float)IMAGEHEIGHT,
			(float)IMAGECOUNT / (float)IMAGECOUNT);
	}*/

	// Apply the user provided transformations
	glMultMatrixd(transManager.GetMatrix());

	glTranslatef(-0.5f, -0.5f, -0.5f);

	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, textureID3D);

	
	for (float fIndx = -1.0f; fIndx <= 1.0f; fIndx += SAMPLE_STEP)
	{
		glBegin(GL_QUADS);
		map3DTexture(fIndx);
		glEnd();
	}
	
	
	/*
	for (int i = 0; i < IMAGECOUNT*4; ++i)
	{
		glBegin(GL_QUADS);
		float zIndex = -1.0f + ( (float) i / ((float) IMAGECOUNT*4)) * 2.0f;   // zIndex element -1 to 1
		map3DTexture(zIndex);
		glEnd();
	}
	*/
	glutSwapBuffers();

}

/*
Resizes the viewer window whilst maintaining the aspect ratio
*/
void resize(int width, int height)
{
	if (ORTHO_OR_PERSP)
	{
		setUpOrtho(width, height);

	}
	else
	{
		changePerspective(width, height, false);
	}
}

/*
Create 3D textures from images. All images are in one raw data file
*/
bool initTexturesRaw(std::string filename)
{

	float factor = 1.0f;
	float gradient = (1.0f - ALPHA_SCALE) / (IMAGECOUNT - 1);

	FILE *file;
	file = fopen(filename.c_str(), "rb");

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
	int count = 0;

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
		chRGBABuffer[nIndx * 4 + 3] = (char) (chBuffer[nIndx] * factor) ; 

		if ((nIndx % (IMAGEWIDTH * IMAGEHEIGHT) == 0) && nIndx != 0)
		{
			factor = factor - gradient;
		}
	}

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA,IMAGEWIDTH, IMAGEHEIGHT, IMAGECOUNT, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)chRGBABuffer);
	glBindTexture(GL_TEXTURE_3D, 0);

	int err = glGetError();
	if (err > 0)
	{
		printf("Error %d\n", err);
	}
	fclose(file);

	/*
	Write each frame to a file to view externally (with matlab for example)
	
	std::ofstream ofs;
	std::stringstream frameFilename;

	for (int index = 0; index < IMAGECOUNT; ++index)
	{
		frameFilename.clear();
		frameFilename.str(std::string());
		frameFilename << index+1 << ".raw";
		ofs.open(frameFilename.str());
		ofs.write(chBuffer + index*(IMAGEWIDTH*IMAGEHEIGHT), IMAGEHEIGHT * IMAGEWIDTH);
		ofs.close();
	}
	*/
	
	delete[] chRGBABuffer;
	delete[] chBuffer;
	return true;
}


/*
Keyboard callback. Rotates image with W-A-S-D and resets with "backspace" (This can be changed)
*/
void keyboard(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W') {
		transManager.Rotate(1.0f, 0, 0);
		glutPostRedisplay();
	}
	else if (key == 's' || key == 'S') {
		transManager.Rotate(-1.0f, 0, 0);
		glutPostRedisplay();
	}
	else if (key == 'a' || key == 'A') {
		transManager.Rotate(0, 1.0f, 0);
		glutPostRedisplay();
	}
	else if (key == 'd' || key == 'D') {
		transManager.Rotate(0, -1.0f, 0);
		glutPostRedisplay();
	}
	else if (key == RESET_KEY)
	{
		transManager.ResetRotation();
		glutPostRedisplay();
	}

	// change perspective
	
	else if (key == 'y' || key == 'Y')
	{
		cameraToggle = cameraToggle ^ 1;
		if (cameraToggle)
		{
			std::cout << "Camera controls selected" << std::endl;
		}
		else
		{
			std::cout << "Perspective controls selected" << std::endl;

		}
	}

	else if (key == 'u' || key == 'U')
	{
		if (!cameraToggle){fovy = fovy + 1.0f;}
		else{EYE_X += 1;}
		changePerspective(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	}
	else if (key == 'j' || key == 'J')
	{
		if (!cameraToggle){ fovy = fovy - 1.0f; }
		else { EYE_X -= 1; }
		changePerspective(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	else if (key == 'i' || key == 'I')
	{
		if (!cameraToggle){ minZ = minZ + 0.1f; }
		else{ EYE_Y += 1; }
		changePerspective(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	else if (key == 'k' || key == 'K')
	{
		if (!cameraToggle) {minZ = minZ - 0.1f;}
		else{ EYE_Y -= 1;  }
		changePerspective(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	else if (key == 'o' || key == 'O')
	{
		if (!cameraToggle) { maxZ = maxZ + 1.0f; }
		else { EYE_Z += 1; }
		changePerspective(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	else if (key == 'l' || key == 'L')
	{
		if (!cameraToggle) { maxZ = maxZ - 1.0f; }
		else { EYE_Z -= 1;  }
		changePerspective(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	else if (key == 'p' || key == 'P')
	{	
		if (ORTHO_OR_PERSP == 0) 
		{ 
			ORTHO_OR_PERSP = 1; 
			setUpOrtho(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			std::cout << "Orthographic projection enabled" << std::endl;
		}
		else 
		{ 
			ORTHO_OR_PERSP = 0; 
			changePerspective(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			std::cout << "Perspective enabled "<< std::endl;  
		}
		glutPostRedisplay();
	}
}

/*
Mouse callback. Used to rotate via mouse
*/
void mouseDrag(int x, int y)
{
	if ( ((x - mouseX) != 0) || ( (y - mouseY) != 0))
	{
		transManager.Rotate((float)(mouseY - y), (float)(mouseX - x), 0.0f);
		mouseX = x;
		mouseY = y;
		glutPostRedisplay();
	}
}

/*
Mouse callback occurs when mouse is clicked
*/
void mouseClick(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;
}

/*
Map 3D texture for display during rendering
*/
void map3DTexture(float textureIndex, int option)
{
	if (option == 0)
	{
		glTexCoord3f(0.0f, 0.0f, ((float)textureIndex + 1.0f) / 2.0f);
		glVertex3f(-dViewPortSize, -dViewPortSize, textureIndex);  // bottom left corner + z index
		glTexCoord3f(1.0f, 0.0f, ((float)textureIndex + 1.0f) / 2.0f);
		glVertex3f(dViewPortSize, -dViewPortSize, textureIndex); // top left corner + z index
		glTexCoord3f(1.0f, 1.0f, ((float)textureIndex + 1.0f) / 2.0f);
		glVertex3f(dViewPortSize, dViewPortSize, textureIndex);  // top right corner + z index
		glTexCoord3f(0.0f, 1.0f, ((float)textureIndex + 1.0f) / 2.0f);
		glVertex3f(-dViewPortSize, dViewPortSize, textureIndex); // bottom right corner + z index
	}
	else if (option == 2)
	{
		//if (textureIndex < 0)
		//{
			glTexCoord3f(0.0f, 0.0f, ((float)textureIndex + 1.0f) / 2.0f);
			glVertex3f(textureIndex, -dViewPortSize, -dViewPortSize);  // bottom left corner + z index
			glTexCoord3f(1.0f, 0.0f, ((float)textureIndex + 1.0f) / 2.0f);
			glVertex3f(textureIndex, -dViewPortSize, dViewPortSize); // top left corner + z index
			glTexCoord3f(1.0f, 1.0f, ((float)textureIndex + 1.0f) / 2.0f);
			glVertex3f(textureIndex, dViewPortSize, dViewPortSize);  // top right corner + z index
			glTexCoord3f(0.0f, 1.0f, ((float)textureIndex + 1.0f) / 2.0f);
			glVertex3f(textureIndex, dViewPortSize, -dViewPortSize); // bottom right corner + z index
		//}
	/*else
		{
			glTexCoord3f(0.0f, 0.0f, ((float)textureIndex + 1.0f) / 2.0f);
			glVertex3f(-textureIndex, -dViewPortSize, -dViewPortSize);  // bottom left corner + z index
			glTexCoord3f(1.0f, 0.0f, ((float)textureIndex + 1.0f) / 2.0f);
			glVertex3f(-textureIndex, -dViewPortSize, dViewPortSize); // top left corner + z index
			glTexCoord3f(1.0f, 1.0f, ((float)textureIndex + 1.0f) / 2.0f);
			glVertex3f(-textureIndex, dViewPortSize, dViewPortSize);  // top right corner + z index
			glTexCoord3f(0.0f, 1.0f, ((float)textureIndex + 1.0f) / 2.0f);
			glVertex3f(-textureIndex, dViewPortSize, -dViewPortSize); // bottom right corner + z index
		}*/
	}
}

/*
Parse the configuration file for options
Returns true if all paramters are successfully parsed, false otherwise
*/
bool parseOptions(std::string filename)
{
	int count = 0;
	std::string line;
	std::ifstream configFile(filename.c_str());
	if (configFile.is_open())
	{
		while (std::getline(configFile, line))
		{
			if (line.length() != 0 && line[0] != DELIMTER)
			{
				++count;

				switch (count)
				{
				case(pIMAGEWIDTH) :
					std::stringstream(line) >> IMAGEWIDTH;
					std::cout << "Image width: " << IMAGEWIDTH << std::endl;
					break;
				case(pIMAGEHEIGHT) :
					std::stringstream(line) >> IMAGEHEIGHT;
					std::cout << "Image height: " << IMAGEHEIGHT << std::endl;
					break;
				case(pIMAGECOUNT) :
					std::stringstream(line) >> IMAGECOUNT;
					std::cout << "Image count: " << IMAGECOUNT << std::endl;
					break;
				case(pALPHA_THRESHOLD) :
					std::stringstream(line) >> ALPHA_THRESHOLD;
					std::cout << "Alpha threshold: " << ALPHA_THRESHOLD << std::endl;
					break;
				case(pSAMPLE_STEP) :
					std::stringstream(line) >> SAMPLE_STEP;
					std::cout << "Sample step: " << SAMPLE_STEP << std::endl;
					break;
				case(pDATAFILE) :
					DATAFILE = line;
					std::cout << "Data file: " << DATAFILE << std::endl;
					break;
				case(pWINDOW_TITLE) :
					WINDOW_TITLE = line;
					std::cout << "Window title: " << WINDOW_TITLE << std::endl;
					break;
				case(pRESET_KEY) :
					std::stringstream(line) >> RESET_KEY;
					std::cout << "Reset key (ASCII): " << RESET_KEY << std::endl;
					break;
				case(pALPHA_SCALE) :
					std::stringstream(line) >> ALPHA_SCALE;
					std::cout << "Alpha scaling: " << ALPHA_SCALE << std::endl;
					break;
				case(pORTOHO_OR_PERSP) :
					std::stringstream(line) >> ORTHO_OR_PERSP;
					std::cout << "Orthographic/Perspective: " << ORTHO_OR_PERSP << std::endl;
					break;
				case(pEYE_X):
					std::stringstream(line) >> EYE_X;
					std::cout << "Eye x: " << EYE_X << std::endl;
					break;
				case(pEYE_Y) :
					std::stringstream(line) >> EYE_Y;
					std::cout << "Eye y: " << EYE_Y << std::endl;
					break;
				case(pEYE_Z) :
					std::stringstream(line) >> EYE_Z;
					std::cout << "Eye z: " << EYE_Z << std::endl;
					break;
				case (pRAW_OR_IMAGES):
					std::stringstream(line) >> RAW_OR_IMAGES;
					std::cout << "Raw/Images: " << RAW_OR_IMAGES << std::endl;
					break;
				default:
					break;
				}
			}
		}
		configFile.close();
	}

	if (count >= pLAST)
	{
		return true;
	}
	return false;
}


bool initTexturesFiles(std::string filename)
{
	cv::Mat image;

	// Holds the luminance buffer
	unsigned char* chRGBABuffer = new unsigned char[IMAGEWIDTH * IMAGEHEIGHT * IMAGECOUNT * 4]; // divide by 16 due to 25% reduction

	//Only create 1 3D texture now
	glGenTextures(1, (GLuint*)&textureID3D);

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

	int index = 0;

	for (int i = 0; i < IMAGECOUNT; ++i)
	{
		int loadIndex;

		loadIndex = IMAGECOUNT - i - 1;

		std::string filePath = filename + getName3(loadIndex) + ".JPG"; // load in reverse order
		std::cout << "Opening: " << filePath << std::endl;

		image = cv::imread(filePath, CV_LOAD_IMAGE_GRAYSCALE);

		if (!image.data)                              // Check for invalid input
		{
			fprintf(stderr, "Could not open or find image\n");
			return false;
		}

		for (int i = 0; i < image.rows; i++){
			for (int j = 0; j < image.cols; j++){

				chRGBABuffer[index * 4] = image.at<uchar>(i, j);
				chRGBABuffer[index * 4 + 1] = image.at<uchar>(i, j);
				chRGBABuffer[index * 4 + 2] = image.at<uchar>(i, j);
				chRGBABuffer[index * 4 + 3] = image.at<uchar>(i, j);
				index += 1;
			}


		}

	}

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, IMAGEWIDTH, IMAGEHEIGHT , IMAGECOUNT, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)chRGBABuffer);
	glBindTexture(GL_TEXTURE_3D, 0);
	


	delete[] chRGBABuffer;
//	delete[] chBuffer;
	return true;
}

std::string getName3(int number)
{
	std::stringstream nameStream;
	nameStream << "000" << number;
	std::string toReturn = nameStream.str();
	return toReturn.substr(toReturn.length() - 3, 3);
}

/*
Sets up the perspective projection
*/
void changePerspective(int width, int height, bool verbose)
{
	if (!ORTHO_OR_PERSP)
	{
		glViewport(0, 0, (GLsizei)width, (GLsizei)height);		// Set our viewport to the size of our window  
		glMatrixMode(GL_PROJECTION);							// Switch to the projection matrix so that we can manipulate how our scene is viewed  
		glLoadIdentity();										// Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)  
		gluPerspective(fovy,
			(GLfloat)width / (GLfloat)height,					//aspect ratio
			minZ,
			maxZ);

		if (verbose)
		{
			std::cout << "fovy: " << fovy << " minZ: " << minZ << " maxZ: " << maxZ << " Eye X: " << EYE_X << " Eye Y: " << EYE_Y << " EYE Z: " << EYE_Z << std::endl;
		}

		glMatrixMode(GL_MODELVIEW);								// Switch back to the model view matrix, so that we can start drawing shapes correctly  
		glLoadIdentity();
		glutPostRedisplay();
	}
}

/*
Sets up orthographic projection
*/
void setUpOrtho(int width, int height)
{

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