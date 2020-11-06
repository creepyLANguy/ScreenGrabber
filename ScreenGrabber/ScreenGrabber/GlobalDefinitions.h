#pragma once

#include <opencv2/opencv.hpp>
#include "Colourspaces.hpp"
#include "MySocket.h"
#include <Windows.h>

#define WIN32_LEAN_AND_MEAN

using namespace std;
using namespace cv;

bool keepRunning = true;

const wchar_t* kApplicationName = L"Project Luna";
const char* kVisualiserWindowName = "Project Luna - Debug Visualiser";
const char* kScriptWindowName = "Project Luna - Script Animation Visualiser";

const char* kHostsFileName = "hosts.ini";
const char* kPortsFileName = "ports.ini";

const char* kConfigFileName = "config.ini";
const char kConfigDelim = ' ';
const char kConfigCommentDelim = ';';

const char kScriptDelim = ' ';
const char kScriptAnimationStepsDelim = 's';
const char kScriptAnimationDelayDelim = 'd';
const char kScriptAnimationRepeatDelim = 'x';

constexpr auto imageType = CV_8UC4;

vector<MySocket> tempSockets, sockets;

double (*deltaEFunc)(const LAB&, const LAB&) = nullptr;
void (*runFunc)() = nullptr;

//Device context stuffs
vector <MONITORINFOEX> displays;
HWND hwnd = GetDesktopWindow();
HDC hwindowDC = nullptr;
HDC hwindowCompatibleDC = nullptr;
HBITMAP hbwindow = nullptr;
BITMAPINFOHEADER bi;
//


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


enum class Mode
{
  CONFIG = -1,
  BLACKOUT = 0,
  PRIMARYDISPLAY = 1,
  IMAGEFILE = 2,
  IMAGESEQUENCE = 3,
  SCRIPT = 4,
};


enum class ReservedIndex
{
  CONFIGURE_SERVER = 250,
  UPDATE_ALL_LEDS = 255  
};
