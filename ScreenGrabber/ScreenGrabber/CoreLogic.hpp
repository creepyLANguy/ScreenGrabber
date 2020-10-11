#pragma once

#include "Defines.h"
#include "ChunkInitHelper.hpp"
#include "VectorUtils.hpp"
#include "DeltaE.hpp"

inline bool HasLEDRecentlyBeenUpdated(const int chunkIndex, vector<DWORD>& ledUpdateTracker)
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


inline int GetLuminance(const BorderChunk& chunk)
{
  const int val =
    lumi.r * static_cast<float>(chunk.r) +
    lumi.g * static_cast<float>(chunk.g) +
    lumi.b * static_cast<float>(chunk.b);
  return val;
}


inline inline bool StripAwayOutlierHelper(int& candidateValue, const int comparableValue1, const int comparableValue2)
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
inline void StripAwayOutlier(BorderChunk& chunk)
{
  if (StripAwayOutlierHelper(chunk.r, chunk.g, chunk.b))
  {
    return;
  }
  if (StripAwayOutlierHelper(chunk.g, chunk.r, chunk.b))
  {
    return;
  }
  if (StripAwayOutlierHelper(chunk.b, chunk.g, chunk.r))
  {
    return;
  }
}


inline bool isWhite(const BorderChunk& chunk)
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


inline void ApplyWhiteBrightnessModifier(BorderChunk& chunk)
{
  if (whiteBrightnessModifier == 0)
  {
    return;
  }

  chunk.r += whiteBrightnessModifier;
  chunk.g += whiteBrightnessModifier;
  chunk.b += whiteBrightnessModifier;

  chunk.r = chunk.r > 255 ? 255 : chunk.r;
  chunk.r = chunk.r < 0 ? 0 : chunk.r;
  chunk.g = chunk.g > 255 ? 255 : chunk.g;
  chunk.g = chunk.g < 0 ? 0 : chunk.g;
  chunk.b = chunk.b > 255 ? 255 : chunk.b;
  chunk.b = chunk.b < 0 ? 0 : chunk.b;  
}


//Try and sanitise the colour info for a more clearer and simpler rendereing on the strip. 
//
//Some approaches : 
//Zero out all vals to black if the total luminosity is weak so that we aren't backlighting a dark part of the scene.
//Remove weakest value if the overall colour is not considered to be white.
//
//Make sure we're only zero-ing out an outlying val if its distance from both the other vals exceeds outlierDiffThresh, 
//else we'd be altering the hue.
//
inline void FilterChunk(BorderChunk& chunk)
{
  const int luminance = GetLuminance(chunk);

  //Don't mess with individual vals if the colour is white overall.            
  if (isWhite(chunk))
  {
    if (luminance < whiteLuminanceThresh)
    {
      chunk.r = chunk.g = chunk.b = 0;
    }
    else
    {
      ApplyWhiteBrightnessModifier(chunk);
    }

    return;
  }

  if (luminance < colourLuminanceThresh)
  {
    chunk.r = chunk.g = chunk.b = 0;
    return;
  }

  //Remove weakest value if the overall colour is not considered to be white.
  StripAwayOutlier(chunk);
}


inline void SetBrightness(vector<BorderChunk>& borderChunks)
{
  for (BorderChunk& chunk : borderChunks)
  {
    chunk.r *= brightnessPercentage;
    chunk.g *= brightnessPercentage;
    chunk.b *= brightnessPercentage;
  }
}


inline void RemoveIdenticalChunks(vector<BorderChunk>& chunks, vector<BorderChunk>& previousChunks)
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


inline void GetLab(BorderChunk& chunk, LAB& lab)
{
  RGB rgb;
  rgb.r = chunk.r;
  rgb.g = chunk.g;
  rgb.b = chunk.b;

  XYZ xyz;
  RgbToXyz(rgb, xyz);

  XyzToLab(xyz, lab);
}


inline void GetDeltaE(BorderChunk& chunk1, BorderChunk& chunk2, double& deltae)
{
  LAB lab1;
  GetLab(chunk1, lab1);
  LAB lab2;
  GetLab(chunk2, lab2);
  deltae = deltaEFunc(lab1, lab2);
}


inline void RemoveSimilarChunks(
  vector<BorderChunk>& chunks,
  vector<BorderChunk>& previousChunks
)
{
  if (deltaEThresh <= 0)
  {
    return;
  }

  for (int i = chunks.size() - 1; i >= 0; --i)
  {
    double deltae = 0;
    GetDeltaE(chunks[i], previousChunks[chunks[i].index], deltae);

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


inline void SetAverageRGBValues(vector<BorderChunk>& borderChunks, Mat& mat)
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
        sum_r += pixelPtr[y * mat.cols * cn + x * cn + 2];
        sum_g += pixelPtr[y * mat.cols * cn + x * cn + 1];
        sum_b += pixelPtr[y * mat.cols * cn + x * cn + 0];

        ++pixelCount;
      }
    }

    chunk.r = sum_r / pixelCount;
    chunk.g = sum_g / pixelCount;
    chunk.b = sum_b / pixelCount;
  }
}


inline void OptimiseTransmitWithDelta(
  vector<BorderChunk>& borderChunks,
  vector<BorderChunk>& previousChunks,
  vector<BorderChunk>& limitedChunks
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
    RemoveSimilarChunks(limitedChunks, previousChunks);
  }
}


//TODO
//Allow for specific window to be captured.
inline void SetWindowHandle()
{
  hwnd = GetDesktopWindow();
}


inline void InitialiseDeviceContextStuffs(const int bitmap_width, const int bitmap_height)
{
  hwindowDC = GetDC(hwnd);
  hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);

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


inline void CleanUpDeviceContextStuffs()
{
  DeleteObject(hbwindow);
  DeleteDC(hwindowCompatibleDC);
  ReleaseDC(hwnd, hwindowDC);
}


inline void GrabScreen(Mat& mat, const Rect& rect, const int bitmap_width, const int bitmap_height)
{
  //Use the previously created device context with the bitmap
  SelectObject(hwindowCompatibleDC, hbwindow);
  //Copy from the window device context to the bitmap device context
  StretchBlt(
    hwindowCompatibleDC, 0, 0, bitmap_width, bitmap_height,
    hwindowDC, rect.x, rect.y, rect.width, rect.height, SRCCOPY);
  //copy from hwindowCompatibleDC to hbwindow
  GetDIBits(hwindowCompatibleDC, hbwindow,
    0, bitmap_height, mat.data, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);
}


//For sports and news feed strips at the edge of the screen - we want to ignore these.
inline void ReduceRectByBuffers(RECT& rect)
{
  const int height = rect.bottom - rect.top;
  const int width = rect.right - rect.left;

  const int bottomShift = lowerBuffer * height;
  const int topShift = upperBuffer * height;
  const int leftShift = leftBuffer * width;
  const int rightShift = rightBuffer * width;

  rect.bottom -= bottomShift;
  rect.top += topShift;
  rect.left += leftShift;
  rect.right -= rightShift;
}


inline void TrimRectToRatio(RECT& rect)
{
  const float aspectRatio = (width) / static_cast<float>(height);

  const int display_width = rect.right - rect.left;
  const int display_height = rect.bottom - rect.top;

  const int logical_width = aspectRatio * display_height;
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


inline void GetRect_Display(RECT& rect)
{
  GetClientRect(hwnd, &rect);
  TrimRectToRatio(rect);
  ReduceRectByBuffers(rect);
}


inline void BroadcastPayload(unsigned int& payload)
{
  for (const MySocket& socket : sockets)
  {
    socket.Send(&payload);
  }
}