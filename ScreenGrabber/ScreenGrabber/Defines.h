#ifndef DEFINES_H
#define DEFINES_H

#include <fstream>
#include <Windows.h>
#include "MySocket.h"
#include <iostream>

//AL.
//Try compiling a version without the opencv stuff.
//Maybe just use that EasyBMP
#include <opencv2/highgui/highgui.hpp>

#define WIN32_LEAN_AND_MEAN

#define DEBUG_FPS
#define DEBUG_VISUAL

#define SECOND_MS 1000

using namespace cv;
using namespace std;

enum NoiseType
{
  NONE,
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

const char* kConfigFileName = "config.ini";
const char kDelim = ' ';

//Device context stuffs
const HWND hwnd = GetDesktopWindow();
const HDC hwindowDC = GetDC(hwnd);
const HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
HBITMAP hbwindow;
BITMAPINFOHEADER bi;
//

struct LEDsCollection
{
  int LED_COUNT_UPPER;
  int LED_COUNT_LOWER;
  int LED_COUNT_LEFT;
  int LED_COUNT_RIGHT;
  int LED_COUNT_TOTAL;
};

struct KeyValPair
{
  string key = "";
  string val = "";
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

#endif // DEFINES_H
