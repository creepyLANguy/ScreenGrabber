// ReSharper disable CppClangTidyCppcoreguidelinesNarrowingConversions
// ReSharper disable CppClangTidyClangDiagnosticUnknownEscapeSequence
// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
// ReSharper disable CppClangTidyBugproneNarrowingConversions
// ReSharper disable CppClangTidyClangDiagnosticSignCompare
// ReSharper disable CppClangTidyBugproneExceptionEscape
// ReSharper disable CppInconsistentNaming
// ReSharper disable IdentifierTypo
// ReSharper disable CppUseAuto


#include "Defines.h"
#include "MySocket.h"
#include <iostream>
#include "ConfigHelpers.hpp"
#include "VectorUtils.hpp"
#include "Debug.hpp"
#include "DeltaE.hpp"

#define DEBUG_FPS
#define DEBUG_VISUAL
#define DEBUG_PAYLOAD


inline bool HasLEDRecentlyBeenUpdated(const int chunkIndex, vector<DWORD>& ledUpdateTracker, const int chunkUpdateTimeoutMS)
{
  if (chunkUpdateTimeoutMS <= 0)
  {
    return false;
  }

  const auto now = GetTickCount();
  const auto timeDiff = now - ledUpdateTracker[chunkIndex];
  if (timeDiff < chunkUpdateTimeoutMS)
  {
    return true;
  }

  ledUpdateTracker[chunkIndex] = now;
  return false;
}

int GetLuminance(const BorderChunk& chunk, const Lumi& lumi)
{
  const int val = 
    lumi.r * static_cast<float>(chunk.r) +
    lumi.g * static_cast<float>(chunk.g) +
    lumi.b * static_cast<float>(chunk.b);
  return val;
}


inline bool StripAwayOutlierHelper(int& candidateValue, const int comparableValue1, const int comparableValue2, const int outlierDiffThresh)
{
  if ((candidateValue < comparableValue1) && (candidateValue < comparableValue2))
  {
    const int diff1 = comparableValue1 - candidateValue;
    const int diff2 = comparableValue2 - candidateValue;
    if (diff1 > outlierDiffThresh && diff2 > outlierDiffThresh)
    {
      candidateValue = 0;
      return true;
    }    
  }

  return false;
}


//Make sure we're only zero-ing out an outlying val 
//iff its distance from both the other vals exceeds 
//outlierDiffThresh, else we'd be altering the hue.
void StripAwayOutlier(BorderChunk& chunk, const int outlierDiffThresh)
{
  if (StripAwayOutlierHelper(chunk.r, chunk.g, chunk.b, outlierDiffThresh))
  {
    return;
  }
  if (StripAwayOutlierHelper(chunk.g, chunk.r, chunk.b, outlierDiffThresh))
  {
    return;
  }
  if (StripAwayOutlierHelper(chunk.b, chunk.g, chunk.r, outlierDiffThresh))
  {
    return;
  }
}


bool isWhite(const BorderChunk& chunk, const int whiteDiffThresh)
{
  if (
    abs(chunk.r - chunk.g) > whiteDiffThresh || 
    abs(chunk.r - chunk.b) > whiteDiffThresh ||
    abs(chunk.g - chunk.b) > whiteDiffThresh
    )
  {
      return false;
  }

  return true;
}


//Try and sanitise the colour info for a more clearer and simpler renderening on the strip. 
//
//Some approaches : 
//Zero out all vals to black if the total luminosity is weak so that we aren't backlighting a dark part of the scene.
//Remove weakest value if the overall colour is not considered to be white.
//
//Make sure we're only zero-ing out an outlying val if its distance from both the other vals exceeds outlierDiffThresh, 
//else we'd be altering the hue.
//
void FilterChunk(
  BorderChunk& chunk,
  const int whiteLuminanceThresh,
  const int colourLuminanceThresh,
  const int whiteDiffThresh,
  const int outlierDiffThresh,
  const Lumi& lumi)
{
  const int luminance = GetLuminance(chunk, lumi);

  //Don't mess with individual vals if the colour is white overall.            
  if (isWhite(chunk, whiteDiffThresh))
  {
    if(luminance < whiteLuminanceThresh)
    {
      chunk.r = chunk.g = chunk.b = 0;
    }
    return;
  }

  if (luminance < colourLuminanceThresh)
  {
    chunk.r = chunk.g = chunk.b = 0;
    return;
  }
  
  //Remove weakest value if the overall colour is not considered to be white.
  StripAwayOutlier(chunk, outlierDiffThresh);
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


void RemoveIdenticalChunks(vector<BorderChunk>& chunks, vector<BorderChunk>& previousChunks)
{
  for (int i = chunks.size() - 1; i >= 0; --i)
  {
    const int index = chunks[i].index;
    if (
      chunks[i].r == previousChunks[index].r &&
      chunks[i].g == previousChunks[index].g &&
      chunks[i].b == previousChunks[index].b
      )
    {
      chunks.erase(chunks.begin() + i);
    }
  }
}


void GetLab(BorderChunk& chunk, LAB& lab)
{
  RGB rgb;
  rgb.r = chunk.r;
  rgb.g = chunk.g;
  rgb.b = chunk.b;
    
  XYZ xyz;
  RgbToXyz(rgb, xyz);

  XyzToLab(xyz, lab);
}


void GetDeltaE(BorderChunk& chunk1, BorderChunk& chunk2, double(* deltaEFunc)(const LAB&, const LAB&), double& deltae)
{
  LAB lab1;    
  GetLab(chunk1, lab1);
  LAB lab2;
  GetLab(chunk2, lab2);
  deltae = deltaEFunc(lab1, lab2);
}


void RemoveStaticChunks(
  vector<BorderChunk>& chunks, 
  vector<BorderChunk>& previousChunks, 
  double (*deltaEFunc)(const LAB&, const LAB&), 
  const int deltaEThresh
)
{
  if (deltaEThresh <= 0)
  {
    return;
  }

  for (int i = chunks.size() - 1; i >= 0; --i)
  {
    double deltae = 0;
    GetDeltaE(chunks[i], previousChunks[chunks[i].index], deltaEFunc, deltae);

    if (deltae < deltaEThresh)
    {
      chunks.erase(chunks.begin() + i);      
    }
    else 
    {    
      previousChunks[chunks[i].index] = chunks[i];
    }
  }
}


void SetAverageRGBValues(vector<BorderChunk>& borderChunks, Mat& mat)
{
  for (BorderChunk& chunk : borderChunks)
  {
    int pixelCount = 0;

    int sum_r = 0;
    int sum_g = 0;
    int sum_b = 0;

    auto* pixelPtr = static_cast<uint8_t*>(mat.data);
    const int cn = mat.channels();

    for (int x = chunk.x_start; x < chunk.x_end; ++x)
    {
      for (int y = chunk.y_start; y < chunk.y_end; ++y)
      {
        sum_r += pixelPtr[y*mat.cols*cn + x*cn + 2];
        sum_g += pixelPtr[y*mat.cols*cn + x*cn + 1];
        sum_b += pixelPtr[y*mat.cols*cn + x*cn + 0];

        ++pixelCount;
      }
    }

    chunk.r = sum_r / pixelCount;
    chunk.g = sum_g / pixelCount;
    chunk.b = sum_b / pixelCount;
  }
}


void OptimiseTransmitWithDelta(
  vector<BorderChunk>& borderChunks,
  vector<BorderChunk>& previousChunks,
  vector<BorderChunk>& limitedChunks,
  double (*deltaEFunc)(const LAB&, const LAB&),
  const int deltaEThresh
)
{
  OverwriteVector(borderChunks, limitedChunks);

  if (deltaEFunc == nullptr)
  {
    RemoveIdenticalChunks(limitedChunks, previousChunks);
    OverwriteVector(borderChunks, previousChunks);
  }
  else
  {
    RemoveStaticChunks(limitedChunks, previousChunks, deltaEFunc, deltaEThresh);
  }
}


void InitialiseDeviceContextStuffs(const int bitmap_width, const int bitmap_height)
{
  SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

  //Create a bitmap
  hbwindow = CreateCompatibleBitmap(hwindowDC, bitmap_width, bitmap_height);
  bi.biSize = sizeof(BITMAPINFOHEADER);
  bi.biWidth = bitmap_width;
  bi.biHeight = -bitmap_height;  //makes it draw upside down or not
  bi.biPlanes = 1;
  bi.biBitCount = 32;
  bi.biCompression = BI_RGB;
  bi.biSizeImage = 0;
  bi.biXPelsPerMeter = 0;
  bi.biYPelsPerMeter = 0;
  bi.biClrUsed = 0;
  bi.biClrImportant = 0;
}


void CleanUpDeviceContextStuffs()
{
  DeleteObject(hbwindow); 
  DeleteDC(hwindowCompatibleDC); 
  ReleaseDC(hwnd, hwindowDC); 
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

//AL.
//TODO
//Refactor this.
void InitialiseBorderChunks(
  vector<BorderChunk>& borderChunks, 
  const int bitmap_width, 
  const int bitmap_height, 
  const float borderSamplePercentage, 
  const int originPositionOffset, 
  LEDsCollection& leds)
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
    CopyToVector(chunks_upper, borderChunks);
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
    CopyToVector(chunks_right, borderChunks);
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
    CopyToVector(chunks_lower, borderChunks);
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
    CopyToVector(chunks_left, borderChunks);
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

//For sports and news feed strips at the edge of the screen - we want to ignore these.
void ReduceRectByBuffers(RECT& rect, vector<KeyValPair>& configBlob)
{
  const int height = rect.bottom - rect.top;
  const int width = rect.right - rect.left;

  const int bottomShift = GetProperty_Float("lowerBuffer", 0.0f, configBlob) * height;
  const int topShift = GetProperty_Float("upperBuffer", 0.0f, configBlob) * height;
  const int leftShift = GetProperty_Float("leftBuffer", 0.0f, configBlob) * width;
  const int rightShift = GetProperty_Float("rightBuffer", 0.0f, configBlob) * width;

  rect.bottom -= bottomShift; 
  rect.top += topShift;
  rect.left += leftShift;
  rect.right -= rightShift;
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
  const int width = GetProperty_Int("ratioHorizontal", 16, configBlob);
  const int height = GetProperty_Int("ratioVertical", 9, configBlob);

  const float aspectRatioValue = (width) / static_cast<float>(height);

  return aspectRatioValue;
}


int main(const int argc, char** argv)
{
  vector<KeyValPair> config;
  PopulateConfigBlob(config);

  vector<BorderChunk> borderChunks, previousChunks, limitedChunks;

  RECT rect;
  GetClientRect(hwnd, &rect);
  TrimRectToRatio(rect, GetAspectRatio(config));
  ReduceRectByBuffers(rect, config);

  LEDsCollection leds;
  leds.LED_COUNT_UPPER = GetProperty_Int("led_count_upper", 10, config);
  leds.LED_COUNT_LOWER = GetProperty_Int("led_count_lower", 10, config);
  leds.LED_COUNT_LEFT = GetProperty_Int("led_count_left", 5, config);
  leds.LED_COUNT_RIGHT = GetProperty_Int("led_count_right", 5, config);
  leds.LED_COUNT_TOTAL = leds.LED_COUNT_UPPER + leds.LED_COUNT_LOWER + leds.LED_COUNT_LEFT + leds.LED_COUNT_RIGHT;
  cout << "LED Count: " << leds.LED_COUNT_TOTAL << endl;

  if (
      rect.right - rect.left < leds.LED_COUNT_UPPER ||
      rect.right - rect.left < leds.LED_COUNT_LOWER ||
      rect.bottom - rect.top < leds.LED_COUNT_LEFT ||
      rect.bottom - rect.top < leds.LED_COUNT_RIGHT
      )
  {
    MessageBoxA(
      nullptr,
      "TOO MANY LEDS SPECIFIED - EXCEEDS RESOLUTION OF DISPLAY",
      "ScreenGrabber",
      0);
    return -1;
  }

  const int wait_ms = GetProperty_Int("delay_ms", 0, config);

  const int chunkUpdateTimeoutMS = GetProperty_Int("chunkUpdateTimeoutMS", 0, config);
  vector<DWORD> ledUpdateTracker(leds.LED_COUNT_TOTAL);

  const int downscaler = GetProperty_Int("downscale", 3, config);
  const int bitmap_width = (rect.right - rect.left) / downscaler;
  const int bitmap_height = (rect.bottom - rect.top) / downscaler;

  const float borderSamplePercentage = GetProperty_Float("borderSample", 0.1f, config);
  const int originPositionOffset = GetProperty_Int("origin", 0, config);

  InitialiseBorderChunks(borderChunks, bitmap_width, bitmap_height, borderSamplePercentage, originPositionOffset, leds);

  CopyToVector(borderChunks, previousChunks);

  const float brightnessPercentage = GetProperty_Float("brightness", 1.0f, config);
  
  const int whiteDiffThresh = GetProperty_Float("whiteDiffThresh", 1.0f, config) * 255;
  
  const int outlierDiffThresh = GetProperty_Float("outlierDiffThresh", 1.0f, config) * 255;
  
  const int whiteLuminanceThresh = GetProperty_Float("whiteLuminanceThresh", 0.0f, config) * 255;

  const int colourLuminanceThresh = GetProperty_Float("colourLuminanceThresh", 0.0f, config) * 255;

  const bool optimiseTransmitWithDelta = GetProperty_Int("optimiseTransmitWithDelta", 0, config) == 1;
  
  const int deltaEThresh = GetProperty_Int("deltaEThresh", 0, config);

  const auto deltaEType = static_cast<DeltaEType>(GetProperty_Int("deltaEType", static_cast<int>(DeltaEType::CIE2000), config));
  double (*deltaEFunc)(const LAB&, const LAB&) = nullptr;
  switch (deltaEType) 
  {
  case DeltaEType::CIE76:  deltaEFunc = &Calc76; break;
  case DeltaEType::CIE94:  deltaEFunc = &Calc94; break;
  case DeltaEType::CIE2000:  deltaEFunc = &Calc2000; break;
  }
  
  Lumi lumi;
  lumi.r = GetProperty_Float("lumiR", lumi.r, config);
  lumi.g = GetProperty_Float("lumiG", lumi.g, config);
  lumi.b = GetProperty_Float("lumiB", lumi.b, config);


  MySocket socket;
  if (socket.Initialise() == false)
  {
    MessageBoxA(nullptr,
      "Failed to create socket \r\n\r\n "
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

    for (BorderChunk& chunk : borderChunks)
    {
      FilterChunk(chunk, whiteLuminanceThresh, colourLuminanceThresh, whiteDiffThresh, outlierDiffThresh, lumi);
    }

    if (optimiseTransmitWithDelta)
    {
      OptimiseTransmitWithDelta(borderChunks, previousChunks, limitedChunks, deltaEFunc, deltaEThresh);
    }
    else
    {
      limitedChunks = borderChunks;
    }

#ifdef DEBUG_VISUAL
    vector<int> skippedChunksIndexesBasedOnLastUpdatedTime;
#endif

    for (const BorderChunk chunk : limitedChunks)
    {
      if (HasLEDRecentlyBeenUpdated(chunk.index, ledUpdateTracker, chunkUpdateTimeoutMS))
      {
#ifdef DEBUG_VISUAL
        skippedChunksIndexesBasedOnLastUpdatedTime.push_back(chunk.index);
#endif

        continue;
      }

      unsigned int payload = chunk.index << 24 | chunk.r << 16 | chunk.g << 8 | chunk.b;
      
      //AL.
      //int i = 2; int r = 100; int g = 0; int b = 0;
      //payload = i << 24 | r << 16 | g << 8 | b;
      //

      socket.Send(&payload);

#ifdef DEBUG_PAYLOAD
      PrintPayload(payload);
      PrintChunk(chunk);
#endif
    }

#ifdef DEBUG_FPS
    PrintFramerate();
#endif

#ifdef DEBUG_VISUAL
    ShowVisualisation(mat, borderSamplePercentage, limitedChunks, skippedChunksIndexesBasedOnLastUpdatedTime);
#endif

    if (wait_ms)
    {
      Sleep(wait_ms);
    }
  }


  //AL. 
  //Unreachable. 
  //Meh, not massively important to clean this up, right?
  //CleanUpDeviceContextStuffs();

  //return 0;
}
