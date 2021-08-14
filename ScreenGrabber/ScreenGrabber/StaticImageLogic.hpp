#pragma once

#include "ConfigVariables.h"
#include "CoreLogic.hpp"

inline void RunFileBroadcastLoop(
  vector<BorderChunk>& borderChunks,
  Mat& mat)
{
  vector<BorderChunk> empty_bc;
  vector<int> empty_i;

  while (keepRunning)
  {
    for (const BorderChunk& chunk : borderChunks)
    {
      unsigned int payload = chunk.index << 24 | chunk.r << 16 | chunk.g << 8 | chunk.b;

      if (debug_mockPayload) { GetDebugPayload(payload, chunk.index); }
      if (debug_mockChunks) { GetDebugChunk(const_cast<BorderChunk&>(chunk)); }

      BroadcastPayload(payload);

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
          borderChunks,
          borderChunks,
          empty_i,
          empty_bc,
          debug_chunkData
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

  Mat mat = imread(imageFile);
  if (mat.data == nullptr)
  {
    cout << "Failed to open file : " << imageFile << endl;
    return;
  }
  resize(mat, mat, mat.size() / downscaler);

  InitialiseBorderChunks(borderChunks, mat.cols, mat.rows, borderSamplePercentage, originPositionOffset, leds);

  SetAverageRGBValues(borderChunks, mat);

  ApplyRGBShifts(borderChunks);

  AdjustBrightness(borderChunks);

  RunFileBroadcastLoop(
    borderChunks,
    mat);
}
