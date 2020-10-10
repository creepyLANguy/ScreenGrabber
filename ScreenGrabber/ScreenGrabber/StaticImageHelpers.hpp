#pragma once

#include "ConfigVariables.h"
#include "CoreLogic.hpp"


inline void RunFileBroadcastLoop(
  std::vector<MySocket>& sockets,
  vector<BorderChunk>& borderChunks,
  Mat& mat)
{
  vector<BorderChunk> empty_bc;
  vector<int> empty_i;

  while (true)
  {
    for (const BorderChunk& chunk : borderChunks)
    {
      unsigned int payload = chunk.index << 24 | chunk.r << 16 | chunk.g << 8 | chunk.b;

      if (debug_mockPayload) { GetDebugPayload(payload, chunk.index); }
      if (debug_mockChunks) { GetDebugChunk(const_cast<BorderChunk&>(chunk)); }

      for (const MySocket& socket : sockets)
      {
        socket.Send(&payload);
      }

      if (debug_payload) { PrintPayload(payload); PrintChunk(chunk); IncrementMargin(); }
    }

    UpdateDebugTimer(debug_reportTimeMS, borderChunks.size());

    if (debug_fps_cmd || debug_fps_ide) { PrintFramerate(debug_fps_cmd, debug_fps_ide); }

    if (debug_visual)
    {
      if (debug_drawAllFrames || borderChunks.empty() == false)
      {
        ShowVisualisation(
          mat,
          borderSamplePercentage * debug_blankRegionModifier,
          borderChunks,
          empty_i,
          empty_bc,
          debug_blankVal,
          debug_noiseType
        );
      }
    }

    if (staticImageBroadcastSleepMS < 0)
    {
      return;
    }

    if (staticImageBroadcastSleepMS > 0)
    {
      Sleep(staticImageBroadcastSleepMS);
    }

    if (sleepMS)
    {
      Sleep(sleepMS);
    }
  }
}


inline void RunStaticImageFileCapture()
{
  vector<BorderChunk> borderChunks;

  RECT rect;
  Mat mat = imread(imageFile);
  if (mat.data == nullptr)
  {
    cout << "Failed to open file : " << imageFile << endl;
    return;
  }
  resize(mat, mat, mat.size() / downscaler);

  rect.left = rect.top = 0;
  rect.right = mat.cols;
  rect.bottom = mat.rows;

  const int bitmap_width = (rect.right - rect.left);
  const int bitmap_height = (rect.bottom - rect.top);

  InitialiseBorderChunks(borderChunks, bitmap_width, bitmap_height, borderSamplePercentage, originPositionOffset, leds);

  SetAverageRGBValues(borderChunks, mat);

  if (brightnessPercentage < 1.0f)
  {
    SetBrightness(borderChunks);
  }

  RunFileBroadcastLoop(
    sockets,
    borderChunks,
    mat);
}
