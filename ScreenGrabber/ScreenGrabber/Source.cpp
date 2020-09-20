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
#include "ChunkInitHelper.hpp"
#include "VectorUtils.hpp"
#include "Debug.hpp"
#include "DeltaE.hpp"
#include "InitVariablesHelper.hpp"
#include "SocketHelpers.hpp"


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
  const int whiteDiffThresh,
  const int colourLuminanceThresh,
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


void GrabScreen(Mat& mat, const Rect& rect, const int bitmap_width, const int bitmap_height)
{
  //Use the previously created device context with the bitmap
  SelectObject(hwindowCompatibleDC, hbwindow);
  //Copy from the window device context to the bitmap device context
  StretchBlt(
    hwindowCompatibleDC, 0, 0, bitmap_width, bitmap_height, 
    hwindowDC, rect.x, rect.y, rect.width, rect.height, SRCCOPY);
  //copy from hwindowCompatibleDC to hbwindow
  GetDIBits(hwindowCompatibleDC, hbwindow, 
    0, bitmap_height, mat.data, reinterpret_cast<BITMAPINFO *>(&bi), DIB_RGB_COLORS);
}


//For sports and news feed strips at the edge of the screen - we want to ignore these.
void ReduceRectByBuffers(RECT& rect)
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


void TrimRectToRatio(RECT& rect)
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


void RunLoop(vector<MySocket>& sockets, vector<BorderChunk>& borderChunks, vector<BorderChunk>& previousChunks, vector<BorderChunk>& limitedChunks, vector<DWORD>& ledUpdateTracker, const Rect& simpleRect, const int bitmap_width, const int bitmap_height, Mat& mat)
{
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
      FilterChunk(chunk, whiteLuminanceThresh, whiteDiffThresh, colourLuminanceThresh, outlierDiffThresh, lumi);
    }

    if (optimiseTransmitWithDelta)
    {
      OptimiseTransmitWithDelta(borderChunks, previousChunks, limitedChunks, deltaEFunc, deltaEThresh);
    }
    else
    {
      limitedChunks = borderChunks;
    }

    vector<int> skippedChunksIndexesBasedOnLastUpdatedTime;

    for (const BorderChunk& chunk : limitedChunks)
    {
      if (HasLEDRecentlyBeenUpdated(chunk.index, ledUpdateTracker, chunkUpdateTimeoutMS))
      {
        if (debug_visual) { skippedChunksIndexesBasedOnLastUpdatedTime.emplace_back(chunk.index); }

        continue;
      }

      unsigned int payload = chunk.index << 24 | chunk.r << 16 | chunk.g << 8 | chunk.b;
   
      if (debug_mockPayload) { GetDebugPayload(payload, chunk.index); }
      if (debug_mockChunks) { GetDebugChunk(const_cast<BorderChunk&>(chunk)); }

      for (const MySocket& socket : sockets)
      {
        socket.Send(&payload);
      }

      if (debug_payload) { PrintPayload(payload); PrintChunk(chunk); IncrementMargin(); }
    }

    if (console_fps) { PrintFramerate(); }

    if (debug_visual)
    {
      if (debug_drawAllFrames || limitedChunks.empty() == false)
      {
        ShowVisualisation(
          mat,
          borderSamplePercentage * debug_blankRegionModifier,
          limitedChunks,
          skippedChunksIndexesBasedOnLastUpdatedTime,
          previousChunks,
          debug_blankVal,
          debug_noiseType
        );
      }      
    }

    if (sleepMS)
    {
      Sleep(sleepMS);
    }
  }
}


int main(const int argc, char** argv)
{
  //Very important to init the config blobs and variables immediately.
  InitVariables();

  vector<MySocket> tempSockets, sockets;
  SetupSockets(tempSockets, sockets);

  vector<BorderChunk> borderChunks, previousChunks, limitedChunks;

  vector<DWORD> ledUpdateTracker(leds.LED_COUNT_TOTAL);

  RECT rect;
  GetClientRect(hwnd, &rect);
  TrimRectToRatio(rect);
  ReduceRectByBuffers(rect);
  const Rect simpleRect = { rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };

  const int bitmap_width = (rect.right - rect.left) / downscaler;
  const int bitmap_height = (rect.bottom - rect.top) / downscaler;

  InitialiseBorderChunks(borderChunks, bitmap_width, bitmap_height, borderSamplePercentage, originPositionOffset, leds);

  AppendToVector(borderChunks, previousChunks);

  InitialiseDeviceContextStuffs(bitmap_width, bitmap_height);

  Mat mat(bitmap_height, bitmap_width, imageType);

  RunLoop(
    sockets, 
    borderChunks, 
    previousChunks, 
    limitedChunks, 
    ledUpdateTracker, 
    simpleRect, 
    bitmap_width, 
    bitmap_height,
    mat);

  //Unreachable. 
  //Meh, not massively important to clean this up, right?
  //CleanUpDeviceContextStuffs();

  //return 0;
}
