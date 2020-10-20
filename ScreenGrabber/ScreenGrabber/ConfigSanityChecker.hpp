#pragma once

#include "ConfigVariables.h"

#define WARN(Variable, Is) PrintWarning((#Variable), Is)
#define WARNIF(Variable, Is) Check(Variable, Is, (#Variable))


enum class Is
{
  UnimplementedCheck,
  LessThanZero,
  LessThanOrEqualToZero,
  EqualToZero,
  MoreThanOrEqualToZero,
  MoreThanZero,
  LessThanMin,
  MoreThanMax,
  NotOneOrZero,
  NotAnOption,
};


std::map<Is, const char*> check_map = {
  {Is::UnimplementedCheck, " - CHECK TYPE NOT IMPLEMENTED!"},
  {Is::LessThanZero, " is less than zero."},
  {Is::LessThanOrEqualToZero,  " is less than or equal to zero."},
  {Is::EqualToZero,  " is equal to zero."},
  {Is::MoreThanOrEqualToZero, " is more than or equal to zero."},
  {Is::MoreThanZero, " is more than zero."},
  {Is::LessThanMin, " is likely less than the minimum allowed value."},
  {Is::MoreThanMax,  " is likely more than the maximum allowed value."},
  {Is::NotOneOrZero,  " is not a 1 or 0, as is required for feature toggles."},
  {Is::NotAnOption,  " is not the value of any corresponding options."}
};


inline void PrintWarning(const char* variableName, const Is checkType)
{
  const char* warning = check_map.find(checkType)->second;
  cout << "WARNING:\r\n" << variableName << warning << endl << endl;
}


template <class T>
void Check(const T variable, const Is checkType, const char* variableName)
{
  switch (checkType)
  {
  case Is::LessThanZero:
    if (variable < 0) { PrintWarning(variableName, checkType); }
    break;
  case Is::LessThanOrEqualToZero:
    if (variable <= 0) { PrintWarning(variableName, checkType); }
    break;
  case Is::EqualToZero:
    if (variable == 0) { PrintWarning(variableName, checkType); }
    break;
  case Is::MoreThanOrEqualToZero:
    if (variable >= 0) { PrintWarning(variableName, checkType); }
    break;
  case Is::MoreThanZero:
    if (variable > 0) { PrintWarning(variableName, checkType); }
    break;
  default:
    PrintWarning(variableName, Is::UnimplementedCheck);
    break;
  }
}


inline void CheckConfigValues_General()
{
  WARNIF(leds.LED_COUNT_LEFT, Is::LessThanOrEqualToZero);
  WARNIF(leds.LED_COUNT_RIGHT, Is::LessThanOrEqualToZero);
  WARNIF(leds.LED_COUNT_UPPER, Is::LessThanOrEqualToZero);
  WARNIF(leds.LED_COUNT_LOWER, Is::LessThanOrEqualToZero);
  if (leds.LED_COUNT_TOTAL > 255) { WARN(leds.LED_COUNT_TOTAL, Is::MoreThanMax); }

  WARNIF(originPositionOffset, Is::LessThanZero);

  WARNIF(width, Is::LessThanOrEqualToZero);
  WARNIF(height, Is::LessThanOrEqualToZero);

  WARNIF(borderSamplePercentage, Is::LessThanOrEqualToZero);
  if (borderSamplePercentage > 100) { WARN(borderSamplePercentage, Is::MoreThanMax); }

  WARNIF(lowerBufferPercentage, Is::LessThanZero);
  WARNIF(upperBufferPercentage, Is::LessThanZero);
  WARNIF(leftBufferPercentage, Is::LessThanZero);
  WARNIF(rightBufferPercentage, Is::LessThanZero);

  if (lowerBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(lowerBuffer, Is::MoreThanMax); }
  if (upperBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(upperBuffer, Is::MoreThanMax); }
  if (leftBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(leftBuffer, Is::MoreThanMax); }
  if (rightBufferPercentage > (1.0 - borderSamplePercentage)) { WARN(rightBuffer, Is::MoreThanMax); }

  WARNIF(downscaler, Is::LessThanOrEqualToZero);

  WARNIF(sleepMS, Is::LessThanZero);
  WARNIF(chunkUpdateTimeoutMS, Is::LessThanZero);

  WARNIF(brightnessPercentage, Is::LessThanZero);
  if (brightnessPercentage > 1.0) { WARN(brightnessPercentage, Is::MoreThanMax); }

  WARNIF(whiteDiffThresh, Is::LessThanZero);
  WARNIF(outlierDiffThresh, Is::LessThanZero);
  WARNIF(whiteLuminanceThresh, Is::LessThanZero);
  WARNIF(colourLuminanceThresh, Is::LessThanZero);

  WARNIF(deltaEThresh, Is::LessThanZero);
  if (static_cast<int>(deltaEType) < 0) { WARN(deltaEType, Is::NotAnOption); }
  //< 0 instead of stating SimpleRGBComparison cos it's not explicitly defined
  //in code and is executed when the deltae func is null.

  if (mode < Mode::PRIMARYDISPLAY) { WARN(deltaEType, Is::NotAnOption); }

  WARNIF(animationSteps, Is::LessThanOrEqualToZero);
  WARNIF(animationDelayMS, Is::LessThanZero);
}


inline void CheckConfigValues_Debug()
{
  WARNIF(debug_blankVal, Is::LessThanZero);

  if (debug_noiseType < NoiseType::NONE) { WARN(debug_noiseType, Is::NotAnOption); }

  WARNIF(debug_blankRegionModifier,Is::LessThanZero);

  WARNIF(debug_reportTimeMS, Is::LessThanZero);

  WARNIF(debug_scriptAnimation_cols, Is::LessThanOrEqualToZero);
  WARNIF(debug_scriptAnimation_cols, Is::LessThanOrEqualToZero);
}


inline void CheckConfigValues()
{
  CheckConfigValues_General();
  CheckConfigValues_Debug();
}