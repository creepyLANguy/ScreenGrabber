#pragma once

#include "ConfigVariables.h"

#define WARN(Variable, CheckType) PrintWarning((#Variable), CheckType)
#define WARNIF(Variable, CheckType) Check(Variable, CheckType, (#Variable))


enum class CheckType
{
  NONE,
  LESSTHANZERO,
  LESSTHANOREQUALTOZERO,
  EQUALTOZERO,
  MORETHANOREQUALTOZERO,
  MORETHANZERO,
  LESSTHANMIN,
  MORETHANMAX,
  NOTONEORZERO,
  NOTANOPTION,
};


std::map<CheckType, const char*> check_map = {
  {CheckType::NONE, " - CHECK TYPE NOT IMPLEMENTED!"},
  {CheckType::LESSTHANZERO, " is less than zero."},
  {CheckType::LESSTHANOREQUALTOZERO,  " is less than or equal to zero."},
  {CheckType::EQUALTOZERO,  " is equal to zero."},
  {CheckType::MORETHANOREQUALTOZERO, " is more than or equal to zero."},
  {CheckType::MORETHANZERO, " is more than zero."},
  {CheckType::LESSTHANMIN, " is likely less than the minimum allowed value."},
  {CheckType::MORETHANMAX,  " is likely more than the maximum allowed value."},
  {CheckType::NOTONEORZERO,  " is not a 1 or 0, as is required for feature toggles."},
  {CheckType::NOTANOPTION,  " is not the value of any corresponding options."}
};


inline void PrintWarning(const char* variableName, const CheckType checkType)
{
  const char* warning = check_map.find(checkType)->second;
  cout << "WARNING:\r\n" << variableName << warning << endl << endl;
}


template <class T>
void Check(const T variable, const CheckType checkType, const char* variableName)
{
  switch (checkType)
  {
  case CheckType::LESSTHANZERO:
    if (variable < 0) { PrintWarning(variableName, checkType); }
    break;
  case CheckType::LESSTHANOREQUALTOZERO:
    if (variable <= 0) { PrintWarning(variableName, checkType); }
    break;
  case CheckType::EQUALTOZERO:
    if (variable == 0) { PrintWarning(variableName, checkType); }
    break;
  case CheckType::MORETHANOREQUALTOZERO:
    if (variable >= 0) { PrintWarning(variableName, checkType); }
    break;
  case CheckType::MORETHANZERO:
    if (variable > 0) { PrintWarning(variableName, checkType); }
    break;
  default:
    PrintWarning(variableName, CheckType::NONE);
    break;
  }
}


inline void CheckConfigValues_General()
{
  WARNIF(width, CheckType::LESSTHANOREQUALTOZERO);
  WARNIF(height, CheckType::LESSTHANOREQUALTOZERO);

  WARNIF(borderSamplePercentage, CheckType::LESSTHANOREQUALTOZERO);
  if (borderSamplePercentage > 100) { WARN(borderSamplePercentage, CheckType::MORETHANMAX); }

  WARNIF(lowerBufferPercentage, CheckType::LESSTHANZERO);
  WARNIF(upperBufferPercentage, CheckType::LESSTHANZERO);
  WARNIF(leftBufferPercentage, CheckType::LESSTHANZERO);
  WARNIF(rightBufferPercentage, CheckType::LESSTHANZERO);

  if (lowerBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(lowerBuffer, CheckType::MORETHANMAX); }
  if (upperBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(upperBuffer, CheckType::MORETHANMAX); }
  if (leftBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(leftBuffer, CheckType::MORETHANMAX); }
  if (rightBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(rightBuffer, CheckType::MORETHANMAX); }

  WARNIF(downscaler, CheckType::LESSTHANOREQUALTOZERO);

  WARNIF(sleepMS, CheckType::LESSTHANZERO);
  WARNIF(chunkUpdateTimeoutMS, CheckType::LESSTHANZERO);

  WARNIF(brightnessPercentage, CheckType::LESSTHANZERO);
  if (brightnessPercentage > 100) { WARN(brightnessPercentage, CheckType::MORETHANMAX); }

  WARNIF(whiteDiffThresh, CheckType::LESSTHANZERO);
  WARNIF(outlierDiffThresh, CheckType::LESSTHANZERO);
  WARNIF(whiteLuminanceThresh, CheckType::LESSTHANZERO);
  WARNIF(colourLuminanceThresh, CheckType::LESSTHANZERO);

  WARNIF(deltaEThresh, CheckType::LESSTHANZERO);
  if (static_cast<int>(deltaEType) < 0) { WARN(deltaEType, CheckType::NOTANOPTION); }
  //< 0 instead of stating SimpleRGBComparison cos it's not explicitly defined
  //in code and is executed when the deltae func is null.

  if (captureType < CaptureType::PRIMARYDISPLAY) { WARN(deltaEType, CheckType::NOTANOPTION); }

  WARNIF(animationSteps, CheckType::LESSTHANOREQUALTOZERO);
  WARNIF(animationDelayMS, CheckType::LESSTHANZERO);
}


inline void CheckConfigValues_Debug()
{
  WARNIF(debug_blankVal, CheckType::LESSTHANZERO);

  if (debug_noiseType < NoiseType::NONE) { WARN(debug_noiseType, CheckType::NOTANOPTION); }

  WARNIF(debug_blankRegionModifier,CheckType::LESSTHANZERO);

  WARNIF(debug_reportTimeMS, CheckType::LESSTHANZERO);

  WARNIF(debug_scriptAnimation_cols, CheckType::LESSTHANOREQUALTOZERO);
  WARNIF(debug_scriptAnimation_cols, CheckType::LESSTHANOREQUALTOZERO);
}


inline void CheckConfigValues()
{
  CheckConfigValues_General();
  CheckConfigValues_Debug();
}