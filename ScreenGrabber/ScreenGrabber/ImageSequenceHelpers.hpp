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

  while (myFile.eof() == false)
  {
    string strLine;
    getline(myFile, strLine);
    if (strLine.length() == 0 || strLine[0] == kConfigCommentDelim) { continue; }
    strLine = animationDirectory + strLine;

    Mat mat = imread(strLine);
    if (mat.data == nullptr)
    {
      cout << "Failed to open file : " << strLine << endl;
      continue;
    }
    resize(mat, mat, mat.size() / downscaler);

    images.emplace_back(mat);
    cout << "Added to sequence : " << strLine << endl;
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

  if (step > animationSteps)
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