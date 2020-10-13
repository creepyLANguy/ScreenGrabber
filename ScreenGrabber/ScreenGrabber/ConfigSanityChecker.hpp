#pragma once

#include "ConfigVariables.h"

#define WARN(Variable, Warning) PrintWarning((#Variable), Warning)


const char* lessThanZero = " is less than zero.\r\n";
const char* lessThanOrEqualToZero = " is less than or equal to zero.\r\n";
const char* equalToZero = " is equal to zero.\r\n";
const char* moreThanOrEqualToZero = " is more than or equal to zero.\r\n";
const char* moreThanZero = " is more than zero.\r\n";
const char* lessThanMin = " is likely less than the minimum allowed value.\r\n";
const char* moreThanMax = " is likely more than the maximum allowed value.\r\n";
const char* notOneOrZero = " is not a 1 or 0, as is required for feature toggles.\r\n";
const char* notAnOption = " is not the value of any corresponding options.\r\n";


inline void PrintWarning(const char* variableName, const char* warning)
{
  cout << "WARNING:\r\n" << variableName << warning << endl;
}


inline void CheckConfigValues_General()
{
  //AL.
  //TODO
  
  if (width <= 0) { WARN(width, lessThanOrEqualToZero); }
  if (height <= 0) { WARN(height, lessThanOrEqualToZero); }

  if (borderSamplePercentage <= 0) { WARN(borderSamplePercentage, lessThanOrEqualToZero); }
  if (borderSamplePercentage > 100) { WARN(borderSamplePercentage, moreThanMax); }

  if (lowerBufferPercentage < 0) { WARN(lowerBuffer, lessThanZero); }
  if (upperBufferPercentage < 0) { WARN(upperBuffer, lessThanZero); }
  if (leftBufferPercentage < 0) { WARN(leftBuffer, lessThanZero); }
  if (rightBufferPercentage < 0) { WARN(rightBuffer, lessThanZero); }

  if (lowerBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(lowerBuffer, moreThanMax); }
  if (upperBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(upperBuffer, moreThanMax); }
  if (leftBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(leftBuffer, moreThanMax); }
  if (rightBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(rightBuffer, moreThanMax); }

  if (downscaler <= 0) { WARN(downscaler, lessThanOrEqualToZero); }

  if (sleepMS < 0) { WARN(sleepMS, lessThanZero); }
  if (chunkUpdateTimeoutMS < 0) { WARN(chunkUpdateTimeoutMS, lessThanZero); }

  if (brightnessPercentage < 0) { WARN(brightnessPercentage, lessThanZero); }
  if (brightnessPercentage > 100) { WARN(brightnessPercentage, moreThanMax); }
  
  if (whiteDiffThresh < 0) { WARN(whiteDiffThresh, lessThanZero); }
  if (outlierDiffThresh < 0) { WARN(outlierDiffThresh, lessThanZero); }
  if (whiteLuminanceThresh < 0) { WARN(whiteLuminanceThresh, lessThanZero); }
  if (colourLuminanceThresh < 0) { WARN(colourLuminanceThresh, lessThanZero); }

  if (deltaEThresh < 0) { WARN(deltaEThresh, lessThanZero); }
  if (static_cast<int>(deltaEType) < 0) { WARN(deltaEType, notAnOption); }
  //< 0 instead of stating SimpleRGBComparison cos it's not explicitly defined
  //in code and is executed when the deltae func is null.

  if (captureType < CaptureType::PRIMARYDISPLAY) { WARN(deltaEType, notAnOption); }

  if (animationSteps <= 0) { WARN(animationSteps, lessThanOrEqualToZero); }
  if (animationDelayMS < 0) { WARN(animationDelayMS, lessThanZero); }
}


inline void CheckConfigValues_Debug()
{
  if (debug_blankVal < 0) { WARN(debug_blankVal, lessThanZero); }

  if (debug_noiseType < ::NONE) { WARN(debug_noiseType, notAnOption); }

  if (debug_blankRegionModifier < 0) { WARN(debug_blankRegionModifier, lessThanZero); }

  if (debug_reportTimeMS < 0) { WARN(debug_reportTimeMS, lessThanZero); }

  if (debug_scriptAnimation_cols <= 0) { WARN(debug_scriptAnimation_cols, lessThanOrEqualToZero); }
  if (debug_scriptAnimation_rows <= 0) { WARN(debug_scriptAnimation_rows, lessThanOrEqualToZero); }  
}


inline void CheckConfigValues()
{
  CheckConfigValues_General();
  CheckConfigValues_Debug();
  cout << endl;
}