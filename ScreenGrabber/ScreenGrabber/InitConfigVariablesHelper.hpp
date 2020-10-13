#pragma once

#include "ConfigVariables.h"
#include "ConfigHelpers.hpp"
#include "Debug.hpp"
#include "RunFuncs.hpp"

inline void InitConfigVariables_Debug()
{
  PopulateConfigBlob(kDebugConfigFileName, debug_config);

  debug_fps_cmd = GetProperty_Bool("debug_fps_cmd", false, debug_config);
  debug_fps_ide = GetProperty_Bool("debug_fps_ide", false, debug_config);
  debug_visual = GetProperty_Bool("draw_visual", false, debug_config);
  debug_payload = GetProperty_Bool("print_payload", false, debug_config);
  debug_drawAllFrames = GetProperty_Bool("drawAllFrames", false, debug_config);
  debug_mockPayload = GetProperty_Bool("mockPayload", false, debug_config);
  debug_mockChunks = GetProperty_Bool("mockChunks", false, debug_config);
  debug_blankVal = GetProperty_Int("blankVal", blankVal_default, debug_config);
  debug_noiseType = static_cast<NoiseType>(GetProperty_Int("noiseType", noiseType_default, debug_config));
  debug_blankRegionModifier = GetProperty_Float("blankRegionModifier", blankRegionModifier_default, debug_config);
  debug_reportTimeMS = GetProperty_Int("reportTimeMS", 200, debug_config);
  debug_scriptAnimation_show = GetProperty_Bool("debug_scriptAnimation_show", true, debug_config);
  debug_scriptAnimation_rows = GetProperty_Int("debug_scriptAnimation_rows", 250, debug_config);
  debug_scriptAnimation_cols = GetProperty_Int("debug_scriptAnimation_cols", 250, debug_config);
}


inline void InitConfigVariables_General()
{
  PopulateConfigBlob(kConfigFileName, config);

  leds.LED_COUNT_UPPER = GetProperty_Int("led_count_upper", 10, config);
  leds.LED_COUNT_LOWER = GetProperty_Int("led_count_lower", 10, config);
  leds.LED_COUNT_LEFT = GetProperty_Int("led_count_left", 5, config);
  leds.LED_COUNT_RIGHT = GetProperty_Int("led_count_right", 5, config);
  leds.LED_COUNT_TOTAL = leds.LED_COUNT_UPPER + leds.LED_COUNT_LOWER + leds.LED_COUNT_LEFT + leds.LED_COUNT_RIGHT;
  cout << "LED Count: " << leds.LED_COUNT_TOTAL << endl << endl;

  sleepMS = GetProperty_Int("sleepMS", 0, config);
  chunkUpdateTimeoutMS = GetProperty_Int("chunkUpdateTimeoutMS", 0, config);

  originPositionOffset = GetProperty_Int("originIndex", 0, config);

  width = GetProperty_Int("ratioHorizontal", 16, config);
  height = GetProperty_Int("ratioVertical", 9, config);

  downscaler = GetProperty_Int("downscale", 3, config);
  borderSamplePercentage = GetProperty_Float("borderSamplePercentage", 0.1f, config);
  lowerBuffer = GetProperty_Float("lowerBuffer", 0.0f, config);
  upperBuffer = GetProperty_Float("upperBuffer", 0.0f, config);
  leftBuffer = GetProperty_Float("leftBuffer", 0.0f, config);
  rightBuffer = GetProperty_Float("rightBuffer", 0.0f, config);

  brightnessPercentage = GetProperty_Float("brightnessPercentage", 1.0f, config);
  whiteBrightnessModifier = GetProperty_Int("whiteBrightnessModifier", 0, config);
  whiteDiffThresh = GetProperty_Float("whiteDiffThreshPercentage", 1.0f, config) * 255;
  outlierDiffThresh = GetProperty_Float("outlierDiffThreshPercentage", 1.0f, config) * 255;
  whiteLuminanceThresh = GetProperty_Float("whiteLuminanceThreshPercentage", 0.0f, config) * 255;
  colourLuminanceThresh = GetProperty_Float("colourLuminanceThreshPercentage", 0.0f, config) * 255;
  optimiseTransmitWithDelta = GetProperty_Bool("optimiseTransmitWithDelta", false, config);
  deltaEThresh = GetProperty_Int("deltaEThresh", 0, config);

  deltaEType = static_cast<DeltaEType>(GetProperty_Int("deltaEType", static_cast<int>(DeltaEType::CIE2000), config));
  switch (deltaEType)
  {
  case DeltaEType::CIE76:  deltaEFunc = &Calc76; break;
  case DeltaEType::CIE94:  deltaEFunc = &Calc94; break;
  case DeltaEType::CIE2000:  deltaEFunc = &Calc2000; break;
  default: cout << "WARNING! DeltaE func not set as invalid value was specified!" << endl << endl;
  }

  lumi.r = GetProperty_Float("lumiR", lumi.r, config);
  lumi.g = GetProperty_Float("lumiG", lumi.g, config);
  lumi.b = GetProperty_Float("lumiB", lumi.b, config);

  captureType = static_cast<CaptureType>(GetProperty_Int("captureType", static_cast<int>(CaptureType::PRIMARYDISPLAY), config));
  switch (captureType)
  {
  case CaptureType::PRIMARYDISPLAY:  runFunc = &RunScreenCapture; break;
  case CaptureType::IMAGEFILE:  runFunc= &RunStaticImageFileCapture; break;
  case CaptureType::IMAGESEQUENCE:  runFunc = &RunImageSequenceAnimation; break;
  case CaptureType::SCRIPT:  runFunc = &RunScriptAnimation; break;
  default: cout << "WARNING! runFunc not set as invalid value was specified!" << endl << endl;
  }

  imageFile = GetProperty_String("imageFile", "", config);
  staticImageBroadcastSleepMS = GetProperty_Int("staticImageBroadcastSleepMS", 0, config);

  animationSteps = GetProperty_Int("animationSteps", 50, config);
  animationDelayMS = GetProperty_Int("animationDelayMS", 50, config);

  scriptFile = GetProperty_String("scriptFile", "", config);
  imageSequenceFile = GetProperty_String("imageSequenceFile", "", config);

}


inline void InitConfigVariables()
{
  InitConfigVariables_Debug();
  InitConfigVariables_General();
}
