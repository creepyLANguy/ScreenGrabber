/*
Format is as follows :

sS
dD
file xN
...

where,
S is animationSteps, which overwrites the value specified in config.ini
D is animationDelayMS, which overwrites the value specified in config.ini
file is the filepath, including file extension and any file structure markup such as backslashes.
N is amount of times this hue will repeat

s d and x values are optional

eg:

s2
d40
images\img1.jpg
images\img1.jpg
images\img2.jpg

is same as :

s2
d40
images\img1.jpg x2
images\img2.jpg
*/

#pragma once

#include <fstream>
#include <iostream>
#include "ConfigVariables.h"
#include "CoreLogic.hpp"
#include "ScriptUtils.hpp"


vector<Mat> images;
double delta = 1.0;
double step = 0;
vector<Mat>::iterator currentImage, nextImage;


inline int GetRepeatPos(const string& strLine)
{
  const int dotPos = strLine.find_last_of(".");
  const int repeatPos = strLine.find_last_of(kScriptAnimationRepeatDelim);
  if ( (dotPos < repeatPos) && (dotPos != repeatPos) )
  {
    return repeatPos;
  }

  return -1;
}

inline bool ReadImageSequence()
{
  cout << "Opening image sequence file : " << imageSequenceFile << endl << endl;

  ifstream myFile;
  myFile.open(imageSequenceFile);
  if (myFile.is_open() == false)
  {
    cout << "Failed to open image sequence file : " << imageSequenceFile << endl;
    return false;
  }

  string strLine;
  while (myFile.eof() == false)
  {
    getline(myFile, strLine);
    strLine = Trim(strLine);

    if (IsCommentOrEmpty(strLine)) { continue; }

    if (CheckForScriptOverWritesInLine(strLine)) { continue; }

    int repeats = 1;
    int repeatPos = GetRepeatPos(strLine);
    if (repeatPos > 0)
    {
      const string repeatString = strLine.substr(repeatPos + 1);
      repeats = repeatString.length() > 0 ? atoi(repeatString.c_str()) : 1;
      strLine = strLine.substr(0, repeatPos);
    }

    Mat mat = imread(strLine);
    if (mat.data == nullptr)
    {
      cout << "Failed to open file : " << strLine << endl;
      continue;
    }
    resize(mat, mat, mat.size() / downscaler);

    for (int i = 0; i < repeats; ++i)
    {
      images.emplace_back(mat);
    }

    cout << "Added to sequence : " << strLine << " x" << repeats << endl;
  }

  myFile.close();

  cout << endl;

  if (images.empty())
  {
    std::cout << "Script yielded no keyframes." << imageSequenceFile << endl;
    return false;
  }

  if (animationSteps <= 0)
  {
    cout << "animationSteps specified problematic." << endl;
    cout << "Value : " << animationSteps << endl;
    cout << "Exiting Program." << endl;
    return false;
  }

  return true;
}


inline void GetNextCompositeImage(Mat& mat)
{
  addWeighted(*currentImage, 1 - (delta * step), *nextImage, delta * step, 0.0, mat);

  ++step;

  if (step >= animationSteps)
  {
    step = 0;
    ++currentImage;
    if (currentImage == images.end()) { currentImage = images.begin(); }
    ++nextImage;
    if (nextImage == images.end()) { nextImage = images.begin(); }
  }
}


inline void RunImageSequenceLoop(
  vector<BorderChunk>& borderChunks,
  vector<BorderChunk>& previousChunks,
  vector<BorderChunk>& limitedChunks,
  vector<DWORD>& ledUpdateTracker,
  Mat& mat)
{
  while (keepRunning)
  {
    GetNextCompositeImage(mat);

    SetAverageRGBValues(borderChunks, mat);

    for (BorderChunk& chunk : borderChunks)
    {
      FilterChunk(chunk);
    }

    if (optimiseTransmitWithDelta)
    {
      OptimiseTransmitWithDelta(borderChunks, previousChunks, limitedChunks);
    }
    else
    {
      limitedChunks = borderChunks;
    }

    ApplyRGBShifts(limitedChunks);

    AdjustBrightness(limitedChunks);

    vector<int> skippedChunksIndexesBasedOnLastUpdatedTime;

    for (const BorderChunk& chunk : limitedChunks)
    {
      if (HasLEDRecentlyBeenUpdated(chunk.index, ledUpdateTracker))
      {
        if (debug_visual) { skippedChunksIndexesBasedOnLastUpdatedTime.emplace_back(chunk.index); }

        continue;
      }

      unsigned int payload = chunk.index << 24 | chunk.r << 16 | chunk.g << 8 | chunk.b;

      if (debug_mockPayload) { GetDebugPayload(payload, chunk.index); }
      if (debug_mockChunks) { GetDebugChunk(const_cast<BorderChunk&>(chunk)); }

      BroadcastPayload(payload);

      if (debug_payload) { PrintPayload(payload); PrintChunk(chunk); IncrementMargin(); }
    }

    UpdateDebugTimer(debug_reportTimeMS, limitedChunks.size());

    if (debug_fps_cmd || debug_fps_ide) { PrintFramerate(debug_fps_cmd, debug_fps_ide); }

    if (debug_visual)
    {
      if (debug_drawAllFrames || limitedChunks.empty() == false)
      {
        ShowVisualisation(
          mat,
          borderChunks,
          limitedChunks,
          skippedChunksIndexesBasedOnLastUpdatedTime,
          previousChunks,
          debug_chunkData,
          debug_isPreWindows10Version2004
        );
      }
    }

    if (animationDelayMS > 0)
    {      
      Sleep(animationDelayMS);
    }

    if (sleepMS)
    {
      Sleep(sleepMS);
    }
  }
}


inline void RunImageSequence()
{
  delta = 1.0 / animationSteps;
  currentImage = images.begin();
  nextImage = currentImage + 1;
  if (nextImage == images.end()) { nextImage = images.begin(); }
  Mat mat;

  vector<BorderChunk> borderChunks, previousChunks, limitedChunks;

  vector<DWORD> ledUpdateTracker(leds.LED_COUNT_TOTAL);

  InitialiseBorderChunks(borderChunks, currentImage->cols, currentImage->rows, borderSamplePercentage, originPositionOffset, leds);

  AppendToVector(borderChunks, previousChunks);

  RunImageSequenceLoop(
    borderChunks,
    previousChunks,
    limitedChunks,
    ledUpdateTracker,
    mat);
}