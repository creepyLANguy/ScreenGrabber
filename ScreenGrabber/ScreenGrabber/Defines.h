#ifndef DEFINES_H
#define DEFINES_H

#include <opencv2/highgui/highgui.hpp>

#include <Windows.h>

#include "MySocket.h"
#define WIN32_LEAN_AND_MEAN

using namespace cv;
using namespace std;

const char* kHostsFileName = "hosts.ini";
const char* kPortsFileName = "ports.ini";

const char* kConfigFileName = "config.ini";
const char kConfigDelim = ' ';

const char* kScriptDelim = " ";

constexpr auto imageType = CV_8UC4;


//Device context stuffs
HWND hwnd = nullptr;
HDC hwindowDC = nullptr;
HDC hwindowCompatibleDC = nullptr;
HBITMAP hbwindow = nullptr;
BITMAPINFOHEADER bi;
//


vector<MySocket> tempSockets, sockets;


struct KeyValPair
{
  string key = "";
  string val = "";
};


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


enum class CaptureType
{
  PRIMARYDISPLAY = 0,
  IMAGEFILE = 1,
  SCRIPT = 2,
};

#endif // DEFINES_H
