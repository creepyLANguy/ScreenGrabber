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

s dand x values are optional

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


vector<Mat> images;
double delta = 1.0;
double step = 0;
vector<Mat>::iterator currentImage, nextImage;


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
    if (strLine.length() == 0 || strLine[0] == kConfigCommentDelim) { continue; }

    if (strLine[0] == kScriptAnimationStepsDelim)
    {
      animationSteps = atoi(strLine.substr(1).c_str());
      cout << "Updated animationSteps as per script." << endl;
      cout << "New Value: " << animationSteps << endl;
      cout << endl;
      continue;
    }

    if (strLine[0] == kScriptAnimationDelayDelim)
    {
      animationDelayMS = atoi(strLine.substr(1).c_str());
      cout << "Updated animationDelayMS as per script." << endl;
      cout << "New Value: " << animationDelayMS << endl;
      cout << endl;
      continue;
    }
  
    //TODO
    //Refactor
    string sd = " " + to_string(kScriptAnimationRepeatDelim);
    int repeats = 1;
    const int dotPos = strLine.find_last_of(".");
    const int repeatPos = strLine.find_last_of(sd);
    const bool foundRepeat = (dotPos < repeatPos) && (dotPos != repeatPos);
    if (foundRepeat)
    {
      string s = strLine.substr(repeatPos+2);      
      repeats = strlen(s.c_str()) > 0 ? atoi(s.c_str()) : 1;
      strLine = strLine.substr(0, repeatPos);
    }
    //

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

  cout << endl;

  if (images.empty())
  {
    std::cout << "Script yielded no keyframes." << imageSequenceFile << endl;
    return false;
  }

  if (animationSteps <= 0)
  {
    cout << "animationSteps specified problematic.\nValue : " << animationSteps << endl;
    cout << "Exiting Program." << endl;
    return false;
  }

  myFile.close();

  return true;
}


inline void GetNextCompositeImage(Mat& mat)
{
  addWeighted(*currentImage, 1 - (delta * step), *nextImage, delta * step, 0.0, mat);

  //imshow("imageSequenceAnimationDebugView", mat);
  //waitKey(animationDelayMS);
  
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
  while (true)
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

    if (brightnessPercentage < 1.0f)
    {
      SetBrightness(limitedChunks);
    }

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
          borderSamplePercentage * debug_blankRegionModifier,
          limitedChunks,
          skippedChunksIndexesBasedOnLastUpdatedTime,
          previousChunks,
          debug_blankVal,
          NoiseType(debug_noiseType & ~BLUR)
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