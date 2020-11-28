#pragma once

#include <opencv2/opencv.hpp>
#include "Colourspaces.hpp"
#include "MySocket.h"
#include <Windows.h>
#include "resource.h"

#define WIN32_LEAN_AND_MEAN

using namespace std;
using namespace cv;

bool keepRunning = true;

const wchar_t* kApplicationName = L"Project Luna";
const char* kVisualiserWindowName = "Project Luna - Debug Visualiser";
const char* kScriptWindowName = "Project Luna - Script Animation Visualiser";

HICON hIconSmall = (HICON)LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0);
HICON hIconLarge = (HICON)LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 256, 256, 0);

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
  _first = -3, //update this as you modify the enums
  RESTART = -3,
  SHUTDOWN = -2,
  CONFIG = -1,
  BLACKOUT = 0,
  PRIMARYDISPLAY = 1,
  SPECIFICDISPLAY = 2,
  IMAGEFILE = 3,
  IMAGESEQUENCE = 4,
  SCRIPT = 5,
  _last = 5, //update this as you modify the enums
};


enum class ReservedIndex
{
  CONFIGURE_SERVER = 250,
  SHUTDOWN_SERVER = 251,
  RESTART_SERVER = 252,
  UPDATE_ALL_LEDS = 255  
};
