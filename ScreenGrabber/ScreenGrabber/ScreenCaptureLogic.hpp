#pragma once

#include "ConfigVariables.h"
#include "CoreLogic.hpp"

inline void RunScreenCaptureLoop(
  vector<BorderChunk>& borderChunks,
  vector<BorderChunk>& previousChunks,
  vector<BorderChunk>& limitedChunks,
  vector<DWORD>& ledUpdateTracker,
  const Rect& simpleRect,
  const int bitmap_width,
  const int bitmap_height,
  Mat& mat)
{
  while (true)
  {
    GrabScreen(mat, simpleRect, bitmap_width, bitmap_height);

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


inline void RunScreenCapture()
{
  vector<BorderChunk> borderChunks, previousChunks, limitedChunks;

  vector<DWORD> ledUpdateTracker(leds.LED_COUNT_TOTAL);

  RECT rect;
  SetWindowHandle();
  GetRect_Display(rect);

  const int bitmap_width = (rect.right - rect.left) / downscaler;
  const int bitmap_height = (rect.bottom - rect.top) / downscaler;

  Mat mat(bitmap_height, bitmap_width, imageType);

  InitialiseBorderChunks(borderChunks, bitmap_width, bitmap_height, borderSamplePercentage, originPositionOffset, leds);

  AppendToVector(borderChunks, previousChunks);

  InitialiseDeviceContextStuffs(bitmap_width, bitmap_height);

  const Rect simpleRect = {
    rect.left,
    rect.top,
    rect.right - rect.left,
    rect.bottom - rect.top
  };

  RunScreenCaptureLoop(
    borderChunks,
    previousChunks,
    limitedChunks,
    ledUpdateTracker,
    simpleRect,
    bitmap_width,
    bitmap_height,
    mat);
}
