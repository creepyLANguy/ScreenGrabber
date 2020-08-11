#ifndef DEFINES_H
#define DEFINES_H

#include <Windows.h>

#include <opencv2/highgui/highgui.hpp>

#define WIN32_LEAN_AND_MEAN

using namespace cv;
using namespace std;


const char* kConfigFileName = "config.ini";
const char kDelim = ' ';


#define  DELTA_FUNC double (*deltaeFunc)(const LAB&, const LAB&) 


//Device context stuffs
const HWND hwnd = GetDesktopWindow();
const HDC hwindowDC = GetDC(hwnd);
const HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
HBITMAP hbwindow;
BITMAPINFOHEADER bi;
//


struct KeyValPair
{
  string key = "";
  string val = "";
};


//DEBUGGUNG
enum NoiseType
{
  NONE,
  LOGO,
  GREY,
  COLOUR,
  SHIFTER_1,
  SHIFTER_2,
  SHIFTER_3,
  INCEPTION,
  NOISETYPE_LAST
};
enum NoiseApplicator
{
  INNER,
  OUTER,
  NOISEAPPLICATOR_LAST
};
int shifter = 0;
const string logoFilename = "logo.bmp";
//


struct LEDsCollection
{
  int LED_COUNT_UPPER;
  int LED_COUNT_LOWER;
  int LED_COUNT_LEFT;
  int LED_COUNT_RIGHT;
  int LED_COUNT_TOTAL;
};


struct BorderChunk
{
  int index = -1;

  int x_start = 0;
  int x_end = 0;
  int y_start = 0;
  int y_end = 0;

  //To hold average rgb values for the chunk
  int r = 0;
  int g = 0;
  int b = 0;
};


struct Lumi 
{
  double r = 0.33f;
  double g = 0.50f;
  double b = 0.17f;
};


#endif // DEFINES_H
