/*
Config.h
Anurag Arnab - 10 January 2014

Contains declarations for the paramters which can be defined by the user
*/

#ifndef CONFIG_H
#define CONFIG_H_

#include <string>

const std::string CONFIG_FILE = "config.txt";
const char DELIMTER = '#'; //delimiter for config file

#define pIMAGEWIDTH 1
#define pIMAGEHEIGHT 2
#define pIMAGECOUNT 3
#define pALPHA_THRESHOLD 4
#define pSAMPLE_STEP 5
#define pDATAFILE 6
#define pRAW_OR_IMAGES 7
#define pWINDOW_TITLE 8
#define pRESET_KEY 9
#define pALPHA_SCALE 10
#define pORTOHO_OR_PERSP 11
#define pEYE_X 12
#define pEYE_Y 13
#define pEYE_Z 14
#define pLAST pEYE_Z

int IMAGEWIDTH;
int IMAGEHEIGHT;
int IMAGECOUNT;

float ALPHA_THRESHOLD;
float SAMPLE_STEP;
float ALPHA_SCALE;

int ORTHO_OR_PERSP;
int RAW_OR_IMAGES;
float EYE_X;
float EYE_Y;
float EYE_Z;

std::string DATAFILE;
std::string WINDOW_TITLE;

int RESET_KEY; // reset rotation button set to backspace key by default

#endif