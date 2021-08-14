//This file is rather horrendous,
//but tis really damn useful for troubleshooting 
//and actually seeing visually what's going on.

#pragma once

#include "GlobalDefinitions.h"
#include <bitset>


inline const char* kDebugConfigFileName = "debug_config.ini";


inline auto zeroHourFPS = GetTickCount();
inline auto zeroHourInfo = GetTickCount();
inline unsigned int currentFrameCount = 0;
inline unsigned int lastDeterminedFramerate = 0;
inline unsigned int currentChunksUpdated = 0;
inline unsigned int lastChunksUpdatedTotal = 0;
inline unsigned int updateOccasions = 0;
inline unsigned int lastUpdateOccasionsTotal = 0;


inline void UpdateDebugTimer(const int reportTimeMS, const unsigned int updatedChunksCount)
{
  ++currentFrameCount;

  if (updatedChunksCount > 0)
  {
    ++updateOccasions;
    currentChunksUpdated += updatedChunksCount;
  }

  const auto now = GetTickCount();

  if (now - zeroHourFPS > 1000)
  {
    lastDeterminedFramerate = currentFrameCount;
    currentFrameCount = 0;
    zeroHourFPS = now;
  }

  if (now - zeroHourInfo > reportTimeMS)
  {
    lastChunksUpdatedTotal = currentChunksUpdated;
    lastUpdateOccasionsTotal = updateOccasions;
    currentChunksUpdated = 0;
    updateOccasions = 0;
    zeroHourInfo = now;
  }
}


inline void PrintFramerate(const bool cmd, const bool ide)
{
  const string str = "FPS:" + to_string(lastDeterminedFramerate) + "\r\n";
  if (cmd)
  {
    cout << "\r\n" << str.c_str() << endl;
  }
  if (ide)
  {
    OutputDebugStringA(str.c_str());
  }
}


struct ChunkDataTextProperties
{
  const Scalar textColour = Scalar(255, 255, 255);
  const HersheyFonts font = FONT_HERSHEY_SIMPLEX;
  const double scale = 0.4;
  const int thickness = 1;
  const LineTypes line = LINE_AA;
};

inline ChunkDataTextProperties chunkDataTextProperties;


struct ChunkIndexText
{
  const Scalar textColour = Scalar(255, 255, 255);
  const HersheyFonts font = FONT_HERSHEY_SIMPLEX;
  const double scale = 0.4 / 1.5;
  const int thickness = 1;
  const LineTypes line = LINE_AA;
};

inline ChunkIndexText chunkIndexText;


inline int marginCounter = 0;
inline const int marginLimit = 5;
inline int marginModifier = 1;
inline const char marginMarker = ' ';
inline string margin;
inline void IncrementMargin()
{
  marginCounter += marginModifier;
  if (marginCounter > marginLimit)
  {
    marginCounter = marginLimit;
    marginModifier = -1;
  }
  else if (marginCounter < 0)
  {
    marginCounter = 0;
    marginModifier = 1;
  }
  margin = string(marginCounter, marginMarker);
}


inline void PrintPayload(const unsigned int& payload)
{
  cout << margin << bitset<32>(payload) << endl;
}

inline const char* sep = " | ";
inline void PrintChunk(const BorderChunk& chunk)
{
  cout << margin << chunk.index << sep << chunk.r << sep << chunk.g << sep << chunk.b << endl << endl;
}


inline void FillMatChunksWithAverageRGB(vector<BorderChunk>& borderChunks, Mat& mat)
{
  auto* pixelPtr = static_cast<uint8_t*>(mat.data);
  const int cn = mat.channels();

  for (const BorderChunk& chunk : borderChunks)
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


inline void RemoveSkippedChunks(vector<BorderChunk>& borderChunks, vector<int>& skippedChunksIndexes)
{
  for (int i = borderChunks.size() - 1; i >= 0; --i)
  {
    for (int j = skippedChunksIndexes.size() - 1; j >= 0; --j)
    {
      if (borderChunks[i].index == skippedChunksIndexes[j])
      {
        borderChunks.erase(borderChunks.begin() + i);
        skippedChunksIndexes.erase(skippedChunksIndexes.begin() + j);
        break;
      }
    }
  }
}

inline double totalChunks = 0;
inline void GetChunkDataStrings(string& stats, string& meter)
{
  const string spacer = "     ";
  const string meterMarker = "=";

  stats += "FPS : ";
  if (lastDeterminedFramerate < 10) { stats += "0"; }
  stats += to_string(lastDeterminedFramerate);
  stats += spacer;

  const unsigned int averageUpdates = (lastUpdateOccasionsTotal > 0) ? (lastChunksUpdatedTotal / lastUpdateOccasionsTotal) : 0;

  stats += "UPF : ";
  if (averageUpdates < 10) { stats += "0"; }
  stats += to_string(averageUpdates);
  stats += spacer;

  const int percentage = static_cast<int>(averageUpdates / totalChunks * 100);
  stats += "%PF : ";
  if (percentage < 10) { stats += "0"; }
  stats += to_string(percentage);
  stats += spacer;

  if (percentage > 0 && percentage < 100) { meter += meterMarker; }
  const int dots = percentage / 10;
  for (int i = 0; i < dots; ++i)
  {
    meter += meterMarker;
  }
}


inline void WriteChunkUpdateSummaryToMat(Mat& mat)
{
  string stats, meter;
  GetChunkDataStrings(stats, meter);

  const Size textRegion = getTextSize(
    stats, 
    chunkDataTextProperties.font, 
    chunkDataTextProperties.scale, 
    chunkDataTextProperties.thickness,
    nullptr
  );

  const Point origin = Point(
    mat.cols/2 - (textRegion.width/2), 
    mat.rows/2
  );

  //drop shadow v1
  putText(
    mat,
    stats,
    Point(origin.x+1, origin.y+1),
    chunkDataTextProperties.font,
    chunkDataTextProperties.scale,
    0,
    chunkDataTextProperties.thickness,
    chunkDataTextProperties.line
  );
  
  //drop shadow v2
  /*
  putText(
    mat,
    s,
    origin,
    chunkDataTextProperties.font,
    chunkDataTextProperties.scale,
    Scalar(100, 100, 100),
    chunkDataTextProperties.thickness * 2,
    chunkDataTextProperties.line
  );
  */

  putText(
    mat,
    stats,
    origin,
    chunkDataTextProperties.font,
    chunkDataTextProperties.scale,
    chunkDataTextProperties.textColour,
    chunkDataTextProperties.thickness,
    chunkDataTextProperties.line
  );

  putText(
    mat,
    meter,
    Point(origin.x + textRegion.width, origin.y + 1),
    chunkDataTextProperties.font,
    chunkDataTextProperties.scale,
    0,
    chunkDataTextProperties.thickness,
    chunkDataTextProperties.line
  );

  putText(
    mat,
    meter,
    Point(origin.x + textRegion.width, origin.y),
    chunkDataTextProperties.font,
    chunkDataTextProperties.scale,
    chunkDataTextProperties.textColour,
    chunkDataTextProperties.thickness,
    chunkDataTextProperties.line
  );
}


inline void WriteChunkIndexesToMat(vector<BorderChunk>& borderChunks, Mat& mat)
{
  for(const BorderChunk& chunk : borderChunks)
  {
    const int x = chunk.x_start + ((chunk.x_end - chunk.x_start) / 2);
    const int y = chunk.y_start + ((chunk.y_end - chunk.y_start) / 2) + 4;

    putText(mat,
      to_string(chunk.index),
      Point( x + 1, y + 1),
      chunkIndexText.font,
      chunkIndexText.scale,
      Scalar(100,100,100),
      chunkIndexText.thickness,
      chunkIndexText.line
    ); //shadow

    putText(mat,
      to_string(chunk.index),
      Point(x, y),
      chunkIndexText.font,
      chunkIndexText.scale,
      chunkIndexText.textColour,
      chunkIndexText.thickness,
      chunkIndexText.line
    );    
  }
}

inline void SetVisualiserWindowProperties(const char* windowName)
{
  HWND hwnd = FindWindowA("Main HighGUI class", windowName);
  SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIconSmall));
  SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIconLarge));

  SetWindowDisplayAffinity(hwnd, WDA_MONITOR); //Prevent visualiser window from being captured. 
  //Note, WDA_EXCLUDEFROMCAPTURE is only supported from Windows 10 Version 2004, but WDA_MONITOR behaves the same in most cases so sticking with that.
}


inline void ShowVisualisation(
  Mat& mat,
  vector<BorderChunk>& preoptimisedChunks, 
  vector<BorderChunk>& borderChunks, 
  vector<int>& skippedChunksIndexes, 
  vector<BorderChunk>& previousChunks,
  bool drawChunkData = false
)
{
  FillMatChunksWithAverageRGB(previousChunks, mat);
  RemoveSkippedChunks(borderChunks, skippedChunksIndexes);
  FillMatChunksWithAverageRGB(borderChunks, mat);
  
  if (drawChunkData == false)
  {
    return;
  }

  // TODO - this assignment only needs to happen once...
  totalChunks = previousChunks.empty() ? borderChunks.size() : previousChunks.size();

  WriteChunkUpdateSummaryToMat(mat);
  WriteChunkIndexesToMat(preoptimisedChunks, mat);

  namedWindow(kVisualiserWindowName, WINDOW_NORMAL);
  imshow(kVisualiserWindowName, mat);
  waitKeyEx(1);

  SetVisualiserWindowProperties(kVisualiserWindowName);

  //user has closed the visualiser so exit the loop and allow program to terminate
  keepRunning = getWindowProperty(kVisualiserWindowName, WND_PROP_VISIBLE) != 0;
}


inline void GetDebugPayload(unsigned int& payload, const int i)
{
  payload = 0;
  int r = 0;
  int g = 0;
  int b = 0;

  //Alternative indexes are red or blue.
  i%2 == 0 ? r = 255 : b = 255;    

  payload = i << 24 | r << 16 | g << 8 | b;
}


inline void GetDebugChunk(BorderChunk& chunk)
{
  chunk.r = chunk.g = chunk.b = 0;

  //Alternative indexes are a dimmer or brighter blue.
  chunk.index%2 == 0 ? chunk.b = 55 : chunk.b = 255;    
}


inline void PrintDetectedDisplays()
{
  cout << "Displays detected: " << endl << endl;

  for (auto display : displays)
  {
    wcout << display.szDevice << endl;
    cout << "left\t:\t" << display.rcMonitor.left << endl;
    cout << "top\t:\t" << display.rcMonitor.top << endl;
    cout << "right\t:\t" << display.rcMonitor.right << endl;
    cout << "bottom\t:\t" << display.rcMonitor.bottom << endl;
    cout << "width\t:\t" << display.rcMonitor.right - display.rcMonitor.left << endl;
    cout << "height\t:\t" << display.rcMonitor.bottom - display.rcMonitor.top << endl;
    const string role = display.dwFlags == MONITORINFOF_PRIMARY ? "true" : "false";
    cout << "primary\t:\t" << role << endl;
    cout << endl << endl;
  }
}
