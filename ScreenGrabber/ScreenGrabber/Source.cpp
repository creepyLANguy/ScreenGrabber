#include "stdafx.h"
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

using namespace cv;
using namespace std;

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


void FillMatChunksWithAverageRGB(vector<BorderChunk>& borderChunks, Mat& mat)
{
  uint8_t* pixelPtr = static_cast<uint8_t*>(mat.data);
  const int cn = mat.channels();

  for (const BorderChunk chunk : borderChunks)
  {
    for (int x = chunk.x_start; x < chunk.x_end; ++x)
    {
      for (int y = chunk.y_start; y < chunk.y_end; ++y)
      {
        pixelPtr[y*mat.cols*cn + x*cn + 2] = chunk.r;
        pixelPtr[y*mat.cols*cn + x*cn + 1] = chunk.g;
        pixelPtr[y*mat.cols*cn + x*cn + 0] = chunk.b;
      }
    }
  }
}


void SetNoiseValues(int& shift1, int& shift2, int& shift3, const NoiseType noiseType)
{
  switch (noiseType)
  {
  case GREY:
    shift1 = shift2 = shift3 = rand();
    break;
  case COLOUR:
    shift1 = rand();
    shift2 = rand();
    shift3 = rand();
    break;
  case SHIFTER_1:
    shift1 = ++shifter;
    shift2 = ++shifter*++shifter;
    shift3 = 100;
    break;
  case SHIFTER_2:
    shift1 = shifter;
    shift2 = shifter * 2;
    shift3 = shifter * 3;
    ++shifter;
    break;
  case SHIFTER_3:
    shift1 = ++shifter*shifter;
    shift2 = ++shifter*shifter;
    shift3 = ++shifter*shifter;
    break;
  case NONE:
  default:
    shift1 = shift2 = shift3 = 0;
    break;
  }
}

void BlankMat(Mat& mat, const float leeway = 0, const int blankVal = 0, const NoiseType noiseType = NONE, const NoiseApplicator noiseApplicator = INNER)
{
  if (noiseType == INCEPTION)  {    return;  }

  uint8_t* pixelPtr = static_cast<uint8_t*>(mat.data);
  const int cn = mat.channels();

  int shift1, shift2, shift3;

  for (int y = mat.rows*leeway; y < mat.rows*(1 - leeway); ++y)
  {      
    if (noiseApplicator == OUTER)
    {
      SetNoiseValues(shift1, shift2, shift3, noiseType);
    }

    for (int x = mat.cols*leeway; x < mat.cols*(1 - leeway); ++x)
    {
      if (noiseApplicator == INNER)
      {
        SetNoiseValues(shift1, shift2, shift3, noiseType);
      }

      pixelPtr[y*mat.cols*cn + x*cn + 2] = blankVal + shift1;
      pixelPtr[y*mat.cols*cn + x*cn + 1] = blankVal + shift2;
      pixelPtr[y*mat.cols*cn + x*cn + 0] = blankVal + shift3;
    }
  }
}


void SetBrightness(vector<BorderChunk>& borderChunks, const float brightness)
{
  for (BorderChunk& chunk : borderChunks)
  {
    chunk.r *= brightness;
    chunk.g *= brightness;
    chunk.b *= brightness;
  }
}


void SetAverageRGBValues(vector<BorderChunk>& borderChunks, Mat& mat)
{

  for (BorderChunk& chunk : borderChunks)
  {
    int pixels = 0;

    int sum_r = 0;
    int sum_g = 0;
    int sum_b = 0;

    uint8_t* pixelPtr = static_cast<uint8_t*>(mat.data);
    const int cn = mat.channels();

    for (int x = chunk.x_start; x < chunk.x_end; ++x)
    {
      for (int y = chunk.y_start; y < chunk.y_end; ++y)
      {
        sum_r += pixelPtr[y*mat.cols*cn + x*cn + 2];
        sum_g += pixelPtr[y*mat.cols*cn + x*cn + 1];
        sum_b += pixelPtr[y*mat.cols*cn + x*cn + 0];

        ++pixels;
      }
    }

    chunk.r = sum_r / pixels;
    chunk.g = sum_g / pixels;
    chunk.b = sum_b / pixels;
  }

}


void InitialiseDeviceContextStuffs(const int bitmap_width, const int bitmap_height)
{
  SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

  //Create a bitmap
  hbwindow = CreateCompatibleBitmap(hwindowDC, bitmap_width, bitmap_height);
  bi.biSize = sizeof(BITMAPINFOHEADER);
  bi.biWidth = bitmap_width;
  bi.biHeight = -bitmap_height;  //this is the line that makes it draw upside down or not
  bi.biPlanes = 1;
  bi.biBitCount = 32;
  bi.biCompression = BI_RGB;
  bi.biSizeImage = 0;
  bi.biXPelsPerMeter = 0;
  bi.biYPelsPerMeter = 0;
  bi.biClrUsed = 0;
  bi.biClrImportant = 0;
}


void GrabScreen(Mat& mat, Rect& rect, const int bitmap_width, const int bitmap_height)
{
  //Use the previously created device context with the bitmap
  SelectObject(hwindowCompatibleDC, hbwindow);
  //Copy from the window device context to the bitmap device context
  StretchBlt(hwindowCompatibleDC, 0, 0, bitmap_width, bitmap_height, hwindowDC, rect.x, rect.y, rect.width, rect.height, SRCCOPY);
  //copy from hwindowCompatibleDC to hbwindow
  GetDIBits(hwindowCompatibleDC, hbwindow, 0, bitmap_height, mat.data, reinterpret_cast<BITMAPINFO *>(&bi), DIB_RGB_COLORS);
}


template <class T>
void CopyVector(vector<T>& src, vector<T>& dest)
{
  for (auto v : src)
  {
    dest.push_back(v);
  }
}


void AdjustChunksForGap_Vertical(vector<BorderChunk>& chunks, int gap)
{
  int indexer = 1;
  while (gap)
  {
    chunks[chunks.size() - indexer].y_start += gap;
    chunks[chunks.size() - indexer].y_end += gap;
    --gap;
    ++indexer;
  }

  for (int i = 0; i < chunks.size() - 1; ++i)
  {
    chunks[i].y_end += chunks[i + 1].y_start - chunks[i].y_end;
  }
}


void AdjustChunksForGap_Horizontal(vector<BorderChunk>& chunks, int gap)
{
  int indexer = 1;
  while (gap)
  {
    chunks[chunks.size() - indexer].x_start += gap;
    chunks[chunks.size() - indexer].x_end += gap;
    --gap;
    ++indexer;
  }

  for (int i = 0; i < chunks.size() - 1; ++i)
  {
    chunks[i].x_end += chunks[i + 1].x_start - chunks[i].x_end;
  }
}


void InitialiseBorderChunks(vector<BorderChunk>& borderChunks, const int bitmap_width, const int bitmap_height, const float borderSamplePercentage, const int originPositionOffset, LEDsCollection& leds)
{
  BorderChunk chunk;

  const int chunk_upper_width = bitmap_width / leds.LED_COUNT_UPPER;
  const int chunk_upper_height = bitmap_height * borderSamplePercentage;
  {
    vector<BorderChunk> chunks_upper;
    for (int i = 0; i < leds.LED_COUNT_UPPER; ++i)
    {
      chunk.x_start = i * chunk_upper_width;
      chunk.x_end = chunk.x_start + chunk_upper_width;

      chunk.y_start = 0;
      chunk.y_end = chunk_upper_height;

      chunks_upper.push_back(chunk);
    }
    const int gapUpper = bitmap_width % leds.LED_COUNT_UPPER;
    if (gapUpper)
    {
      AdjustChunksForGap_Horizontal(chunks_upper, gapUpper);
    }
    CopyVector(chunks_upper, borderChunks);
  }

  {
    vector<BorderChunk> chunks_right;
    const int chunk_right_width = bitmap_width * borderSamplePercentage;
    const int neededRightArea = (bitmap_height - (2 * chunk_upper_height));
    const int chunk_right_height = neededRightArea / leds.LED_COUNT_RIGHT;
    for (int i = 0; i < leds.LED_COUNT_RIGHT; ++i)
    {
      chunk.x_end = bitmap_width;
      chunk.x_start = chunk.x_end - chunk_right_width;

      chunk.y_start = (i * chunk_right_height) + chunk_upper_height;
      chunk.y_end = chunk.y_start + chunk_right_height;

      chunks_right.push_back(chunk);
    }
    const int gapRight = neededRightArea % (neededRightArea / leds.LED_COUNT_RIGHT);
    if (gapRight)
    {
      AdjustChunksForGap_Vertical(chunks_right, gapRight);
    }
    CopyVector(chunks_right, borderChunks);
  }

  {
    vector<BorderChunk> chunks_lower;
    const int chunk_lower_width = bitmap_width / leds.LED_COUNT_LOWER;
    const int chunk_lower_height = bitmap_height * borderSamplePercentage;
    for (int i = 0; i < leds.LED_COUNT_LOWER; ++i)
    {
      chunk.x_start = i * chunk_lower_width;
      chunk.x_end = chunk.x_start + chunk_upper_width;

      chunk.y_end = bitmap_height;
      chunk.y_start = chunk.y_end - chunk_lower_height;

      chunks_lower.push_back(chunk);
    }
    const int gapLower = bitmap_width % leds.LED_COUNT_LOWER;
    if (gapLower)
    {
      AdjustChunksForGap_Horizontal(chunks_lower, gapLower);
    }
    CopyVector(chunks_lower, borderChunks);
  }

  {
    vector<BorderChunk> chunks_left;
    const int chunk_left_width = bitmap_width * borderSamplePercentage;
    const int neededLeftArea = (bitmap_height - (2 * chunk_upper_height));
    const int chunk_left_height = neededLeftArea / leds.LED_COUNT_LEFT;
    for (int i = 0; i < leds.LED_COUNT_LEFT; ++i)
    {
      chunk.x_start = 0;
      chunk.x_end = chunk_left_width;

      chunk.y_start = (i * chunk_left_height) + chunk_upper_height;
      chunk.y_end = chunk.y_start + chunk_left_height;

      chunks_left.push_back(chunk);
    }
    const int gapLeft = neededLeftArea % (neededLeftArea / leds.LED_COUNT_LEFT);
    if (gapLeft)
    {
      AdjustChunksForGap_Vertical(chunks_left, gapLeft);
    }
    CopyVector(chunks_left, borderChunks);
  }

  {
    int i = 0;
    for (BorderChunk& bchunk : borderChunks)
    {
      bchunk.index = (i % leds.LED_COUNT_TOTAL) - originPositionOffset;
      if (bchunk.index < 0)
      {
        bchunk.index += leds.LED_COUNT_TOTAL;
      }
      ++i;
    }
  }
}


void TrimRectToRatio(RECT& rect, const float aspect_ratio)
{
  const int display_width = rect.right - rect.left;
  const int display_height = rect.bottom - rect.top;

  const int logical_width = aspect_ratio * display_height;
  int cropped_width = logical_width;
  int cropped_height = display_height;
  if (logical_width > display_width)
  {
    cropped_width -= (logical_width - display_width);
    cropped_height = (cropped_width / static_cast<float>(logical_width)) * display_height;
  }

  const int x_adjustment = (display_width - cropped_width) / 2;
  const int y_adjustment = (display_height - cropped_height) / 2;
  rect.left += x_adjustment;
  rect.right -= x_adjustment;
  rect.top += y_adjustment;
  rect.bottom -= y_adjustment;
}


float GetAspectRatio(vector<KeyValPair>& configBlob)
{
  float aspectRatioValue = 16 / 9.0f;

  string str = "";
  for (KeyValPair kvp : configBlob)
  {
    if (_strcmpi(kvp.key.c_str(), "ratio") == 0)
    {
      str = kvp.val;
      break;
    }
  }

  if (str.length() == 0)
  {
    return aspectRatioValue;
  }

  const int delimPos = str.find_first_of(':');
  const float width = atoi(str.substr(0, delimPos).c_str());
  const float height = atoi(str.substr(delimPos + 1, str.length()).c_str());

  aspectRatioValue = width / height;

  return aspectRatioValue;
}


/*
int GetCommandLineProperty_Int(const int argc, char** argv, char* propertyName, const int default_return)
{
for (int i = 0; i < argc; ++i)
{
if (!_strcmpi(propertyName, argv[i]))
{
return atoi(argv[++i]);
}
}

return default_return;
}


float GetCommandLineProperty_Float(const int argc, char** argv, char* propertyName, const float default_return)
{
return GetCommandLineProperty_Int(argc, argv, propertyName, (default_return *100)) / 100.0f;
}


char* GetCommandLineProperty_String(const int argc, char** argv, char* propertyName, char* default_return)
{
for (int i = 0; i < argc; ++i)
{
if (!_strcmpi(propertyName, argv[i]))
{
return argv[++i];
}
}

return default_return;
}


/*
const char* GetProperty_String(const char* propertyName, const char* default_return, vector<KeyValPair>& configBlob)
{
for (KeyValPair kvp : configBlob)
{
if (_strcmpi(kvp.key.c_str(), propertyName) == 0)
{
return kvp.val.c_str();
}
}

return default_return;
}
*/


int GetProperty_Int(const char* propertyName, const int default_return, vector<KeyValPair>& configBlob)
{
  for (KeyValPair kvp : configBlob)
  {
    if (_strcmpi(kvp.key.c_str(), propertyName) == 0)
    {
      return atoi(kvp.val.c_str());
    }
  }

  return default_return;
}


float GetProperty_Float(const char* propertyName, const float default_return, vector<KeyValPair>& configBlob)
{
  return GetProperty_Int(propertyName, (default_return * 100), configBlob) / 100.0f;
}


void PopulateConfigBlob(vector<KeyValPair>& configBlob)
{
  ifstream myFile;
  myFile.open(kConfigFileName);
  if (myFile.is_open())
  {
    while (myFile.eof() == false)
    {
      string strLine = "";
      getline(myFile, strLine);
      const int delimPos = strLine.find_first_of(kDelim);
      KeyValPair kvp;
      kvp.key = strLine.substr(0, delimPos); ;
      kvp.val = strLine.substr(delimPos + 1, strLine.length());
      configBlob.push_back(kvp);
    }
  }
  myFile.close();
}


//ratio 21:9 delay 0 downscale 3 lowerBuffer 0 borderSample 10 origin 0 brightness 100
int main(const int argc, char** argv)
{
  vector<KeyValPair> config;
  PopulateConfigBlob(config);

  vector<BorderChunk> borderChunks;

  RECT rect;
  GetClientRect(hwnd, &rect);
  TrimRectToRatio(rect, GetAspectRatio(config));
  rect.bottom -= GetProperty_Int("lowerBuffer", 0, config); //For sports and news feed strips at the bottom of the screen.

  const int WAIT_MS = GetProperty_Int("delay", 0, config);

  LEDsCollection leds;
  leds.LED_COUNT_UPPER = GetProperty_Int("led_count_horizontal", 10, config);
  leds.LED_COUNT_LOWER = leds.LED_COUNT_UPPER;
  leds.LED_COUNT_LEFT = GetProperty_Int("led_count_vertical", 5, config);
  leds.LED_COUNT_RIGHT = leds.LED_COUNT_LEFT;
  leds.LED_COUNT_TOTAL = leds.LED_COUNT_UPPER + leds.LED_COUNT_LOWER + leds.LED_COUNT_LEFT + leds.LED_COUNT_RIGHT;
  cout << "LED Count: " << leds.LED_COUNT_TOTAL << endl;

  if
    (
      rect.right - rect.left < leds.LED_COUNT_UPPER ||
      rect.right - rect.left < leds.LED_COUNT_LOWER ||
      rect.bottom - rect.top < leds.LED_COUNT_LEFT ||
      rect.bottom - rect.top < leds.LED_COUNT_RIGHT
      )
  {
    MessageBoxA(nullptr,
      "TOO MANY LEDS SPECIFIED - EXCEEDS RESOLUTION OF DISPLAY",
      "ScreenGrabber",
      0);
    return -1;
  }

  const int downscaler = GetProperty_Int("downscale", 3, config);
  const int bitmap_width = (rect.right - rect.left) / downscaler;
  const int bitmap_height = (rect.bottom - rect.top) / downscaler;

  const float borderSamplePercentage = GetProperty_Float("borderSample", 0.1f, config);
  const int originPositionOffset = GetProperty_Int("origin", 0, config);

  InitialiseBorderChunks(borderChunks, bitmap_width, bitmap_height, borderSamplePercentage, originPositionOffset, leds);

  const float brightnessPercentage = GetProperty_Float("brightness", 1.0f, config);


#ifdef DEBUG_FPS
  DWORD zeroHour = GetTickCount();
  unsigned int frameCount = 0;
#endif


  MySocket socket;
  if (socket.Initialise() == false)
  {
    MessageBoxA(nullptr,
      "Failed to create socket \r\n\r\n\ "
      "1) Close all instances of the application\r\n "
      "2) Check your config files \r\n "
      "3) Launch the application again.",
      "ScreenGrabber",
      0);
    return -1;
  }


  Rect simpleRect = { rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };

  InitialiseDeviceContextStuffs(bitmap_width, bitmap_height);

  Mat mat(bitmap_height, bitmap_width, CV_8UC4);

  while (true)
  {
    GrabScreen(mat, simpleRect, bitmap_width, bitmap_height);

    SetAverageRGBValues(borderChunks, mat);

    if (brightnessPercentage < 1)
    {
      SetBrightness(borderChunks, brightnessPercentage);
    }

    for (const BorderChunk chunk : borderChunks)
    {
      unsigned int payload = chunk.index << 24 | chunk.r << 16 | chunk.g << 8 | chunk.b;
      socket.Send(&payload);
    }


#ifdef DEBUG_FPS
    ++frameCount;
    if (GetTickCount() - zeroHour > 1000)
    {
      const string str = "FPS:" + to_string(frameCount) + "\r\n";
      OutputDebugStringA(str.c_str());
      zeroHour = GetTickCount();
      frameCount = 0;
    }
#endif

#ifdef DEBUG_VISUAL
    const float leeway = borderSamplePercentage * 2;
    const int blankVal = 150;
    const NoiseType noiseType = NONE;//SHIFTER_1;//static_cast<NoiseType>(rand() % NOISETYPE_LAST);
    const NoiseApplicator noiseApplicator = INNER;//static_cast<NoiseApplicator>(rand() % NOISEAPPLICATOR_LAST);
    BlankMat(mat, leeway, blankVal, noiseType, noiseApplicator);
    FillMatChunksWithAverageRGB(borderChunks, mat);
    const String windowName = "";
    namedWindow(windowName, CV_WINDOW_NORMAL);
    imshow(windowName, mat);
    waitKeyEx(1);
#endif

    if (WAIT_MS)
    {
      Sleep(WAIT_MS);
    }
  }


  //AL. 
  //Unreachable. 
  //Meh, not massively important to clean this up, rigtht?
  DeleteObject(hbwindow);
  DeleteDC(hwindowCompatibleDC);
  ReleaseDC(hwnd, hwindowDC);
}

