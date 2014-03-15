/*
Main.h
Anurag Arnab - 10 January 2014

Header file for main.cpp

*/

#ifndef MAIN_H
#define MAIN_H_

#include <string>

void render(void);
void resize(int, int);
void keyboard(unsigned char key, int x, int y);
void mouseDrag(int x, int y); 
void mouseClick(int button, int state, int x, int y);
void changePerspective(int width, int height, bool verbose = true);
void setUpOrtho(int width, int height);

bool initTexturesRaw(std::string filename);
bool initTexturesFiles(std::string filename);

void map3DTexture(float textureIndex, int option = 0);
bool parseOptions(std::string filename);

std::string getName3(int number);

GLfloat dOrthoSize = 1.0f;
GLfloat dViewPortSize = 1.0f;
int mouseX;
int mouseY;
int textureID3D;

// Default perspective settings
float fovy = 45.0f;
float minZ = 0.1f;
float maxZ = 100.0f;
int cameraToggle = 0; // toggle between 0 and 1 to change camera or perspective

#endif

