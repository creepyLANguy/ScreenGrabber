#pragma once

//TODO
//You can't just dump this spaghett into a 'helper' file
//and pretend it doesn't exist. ~~,'
//Refactor, plox
//

#include <stack>
#include <vector>
#include "GlobalDefinitions.h"
#include "VectorUtils.hpp"


//AL.
//TODO
//refactor pls
void ReverseIndexes_Lower(vector<BorderChunk>& chunks)
{
  stack<int> indexes;
  const int startOfBottomIndex = leds.LED_COUNT_UPPER + leds.LED_COUNT_RIGHT;
  for (int i = startOfBottomIndex; i < startOfBottomIndex + leds.LED_COUNT_LOWER; ++i)
  {
    indexes.push(chunks[i].index);
  }
  for (int i = startOfBottomIndex; i < startOfBottomIndex + leds.LED_COUNT_LOWER; ++i)
  {
    chunks[i].index = indexes.top();
    indexes.pop();
  }
}

void ReverseIndexes_Left(vector<BorderChunk>& chunks)
{
  stack<int> indexes;
  const int startOfLeftIndex = leds.LED_COUNT_UPPER + leds.LED_COUNT_RIGHT + leds.LED_COUNT_LOWER;
  for (int i = startOfLeftIndex; i < startOfLeftIndex + leds.LED_COUNT_LEFT; ++i)
  {
    indexes.push(chunks[i].index);
  }
  for (int i = startOfLeftIndex; i < startOfLeftIndex + leds.LED_COUNT_LEFT; ++i)
  {
    chunks[i].index = indexes.top();
    indexes.pop();
  }
}
//


inline void AdjustChunksForGap_Vertical(vector<BorderChunk>& chunks, int gap)
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


inline void AdjustChunksForGap_Horizontal(vector<BorderChunk>& chunks, int gap)
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


inline void InitialiseBorderChunks(
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

      chunks_upper.emplace_back(chunk);
    }
    const int gapUpper = bitmap_width % leds.LED_COUNT_UPPER;
    if (gapUpper)
    {
      AdjustChunksForGap_Horizontal(chunks_upper, gapUpper);
    }
    AppendToVector(chunks_upper, borderChunks);
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

      chunks_right.emplace_back(chunk);
    }
    const int gapRight = neededRightArea % (neededRightArea / leds.LED_COUNT_RIGHT);
    if (gapRight)
    {
      AdjustChunksForGap_Vertical(chunks_right, gapRight);
    }
    AppendToVector(chunks_right, borderChunks);
  }

  {
    vector<BorderChunk> chunks_lower;
    const int chunk_lower_width = bitmap_width / leds.LED_COUNT_LOWER;
    const int chunk_lower_height = bitmap_height * borderSamplePercentage;
    for (int i = 0; i < leds.LED_COUNT_LOWER; ++i)
    {
      chunk.x_start = i * chunk_lower_width;
      chunk.x_end = chunk.x_start + chunk_lower_width;

      chunk.y_end = bitmap_height;
      chunk.y_start = chunk.y_end - chunk_lower_height;

      chunks_lower.emplace_back(chunk);
    }
    const int gapLower = bitmap_width % leds.LED_COUNT_LOWER;
    if (gapLower)
    {
      AdjustChunksForGap_Horizontal(chunks_lower, gapLower);
    }
    AppendToVector(chunks_lower, borderChunks);
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

      chunks_left.emplace_back(chunk);
    }
    const int gapLeft = neededLeftArea % (neededLeftArea / leds.LED_COUNT_LEFT);
    if (gapLeft)
    {
      AdjustChunksForGap_Vertical(chunks_left, gapLeft);
    }
    AppendToVector(chunks_left, borderChunks);
  }

  /*
  We assume strip is mounted as: origin->top->right->bottom->left
  o --- >
  ^     |
  |     v
  < -----
  so if origin is physically elsewhere, apply an adjustment to the logical origin and indexes of the chunks. 
  */
  {
    int i = 0;
    for (BorderChunk& borderchunk : borderChunks)
    {
      borderchunk.index = (i % leds.LED_COUNT_TOTAL) - originPositionOffset;
      if (borderchunk.index < 0)
      {
        borderchunk.index += leds.LED_COUNT_TOTAL;
      }
      ++i;
    }
  }

  //Else the lights are triggered in reverse on these sides due to continuous nature of physical strip.
  {
    ReverseIndexes_Lower(borderChunks);
    ReverseIndexes_Left(borderChunks);
  }  
}
