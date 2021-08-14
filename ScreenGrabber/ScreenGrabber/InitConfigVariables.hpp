#pragma once

#include "ConfigVariables.h"
#include "ConfigUtils.hpp"
#include "ConfigSanityChecker.hpp"
#include "Debug.hpp"
#include "RunModes.hpp"


inline void InitConfigVariables_Debug()
{
  PopulateConfigBlob(kDebugConfigFileName, debug_config);

  debug_fps_cmd = GetProperty_Bool(debug_fps_cmd_s, false, debug_config);
  debug_fps_ide = GetProperty_Bool(debug_fps_ide_s, false, debug_config);
  debug_visual = GetProperty_Bool(debug_visual_s, false, debug_config);
  debug_chunkData = GetProperty_Bool(debug_chunkData_s, false, debug_config);
  debug_payload = GetProperty_Bool(debug_payload_s, false, debug_config);
  debug_drawAllFrames = GetProperty_Bool(debug_drawAllFrames_s, false, debug_config);
  debug_mockPayload = GetProperty_Bool(debug_mockPayload_s, false, debug_config);
  debug_mockChunks = GetProperty_Bool(debug_mockChunks_s, false, debug_config);
  debug_reportTimeMS = GetProperty_Int(debug_reportTimeMS_s, 200, debug_config);
  debug_scriptAnimation_show = GetProperty_Bool(debug_scriptAnimation_show_s, true, debug_config);
  debug_scriptAnimation_rows = GetProperty_Int(debug_scriptAnimation_rows_s, 250, debug_config);
  debug_scriptAnimation_cols = GetProperty_Int(debug_scriptAnimation_cols_s, 250, debug_config);
}


inline void InitConfigVariables_General()
{
  PopulateConfigBlob(kConfigFileName, config);

  leds.LED_COUNT_UPPER = GetProperty_Int(led_count_upper_s, 10, config);
  leds.LED_COUNT_LOWER = GetProperty_Int(led_count_lower_s, 10, config);
  leds.LED_COUNT_LEFT = GetProperty_Int(led_count_left_s, 5, config);
  leds.LED_COUNT_RIGHT = GetProperty_Int(led_count_right_s, 5, config);
  leds.LED_COUNT_TOTAL = leds.LED_COUNT_UPPER + leds.LED_COUNT_LOWER + leds.LED_COUNT_LEFT + leds.LED_COUNT_RIGHT;
  cout << "LED Count: " << leds.LED_COUNT_TOTAL << endl << endl;

  sleepMS = GetProperty_Int(sleepMS_s, 0, config);
  chunkUpdateTimeoutMS = GetProperty_Int(chunkUpdateTimeoutMS_s, 0, config);

  originPositionOffset = GetProperty_Int(originPositionOffset_s, 0, config);

  width = GetProperty_Int(width_s, 16, config);
  height = GetProperty_Int(height_s, 9, config);

  downscaler = GetProperty_Int(downscaler_s, 3, config);
  borderSamplePercentage = GetProperty_Float(borderSamplePercentage_s, 0.1f, config);
  lowerBufferPercentage = GetProperty_Float(lowerBufferPercentage_s, 0.0f, config);
  upperBufferPercentage = GetProperty_Float(upperBufferPercentage_s, 0.0f, config);
  leftBufferPercentage = GetProperty_Float(leftBufferPercentage_s, 0.0f, config);
  rightBufferPercentage = GetProperty_Float(rightBufferPercentage_s, 0.0f, config);

  physicalStripBrightness = GetProperty_Int(physicalStripBrightness_s, 50, config);
  brightnessPercentage = GetProperty_Float(brightnessPercentage_s, 1.0f, config);
  whiteBrightnessModifier = GetProperty_Int(whiteBrightnessModifier_s, 0, config);

  redShift = GetProperty_Int(redShift_s, 0, config);
  blueShift = GetProperty_Int(blueShift_s, 0, config);
  greenShift = GetProperty_Int(greenShift_s, 0, config);

  whiteDiffThresh = GetProperty_Float(whiteDiffThresh_s, 1.0f, config) * 255;
  outlierDiffThresh = GetProperty_Float(outlierDiffThresh_s, 1.0f, config) * 255;
  whiteLuminanceThresh = GetProperty_Float(whiteLuminanceThresh_s, 0.0f, config) * 255;
  colourLuminanceThresh = GetProperty_Float(colourLuminanceThresh_s, 0.0f, config) * 255;
  optimiseTransmitWithDelta = GetProperty_Bool(optimiseTransmitWithDelta_s, false, config);
  deltaEThresh = GetProperty_Int(deltaEThresh_s, 0, config);

  deltaEType = static_cast<DeltaEType>(GetProperty_Int(deltaEType_s, static_cast<int>(DeltaEType::CIE2000), config));
  switch (deltaEType)
  {
  case DeltaEType::CIE76:  deltaEFunc = &Calc76; break;
  case DeltaEType::CIE94:  deltaEFunc = &Calc94; break;
  case DeltaEType::CIE2000:  deltaEFunc = &Calc2000; break;
  default: cout << "WARNING! DeltaE func not set as invalid value was specified in config!" << endl << endl;
  }

  lumi.r = GetProperty_Float(lumiR_s, lumi.r, config);
  lumi.g = GetProperty_Float(lumiG_s, lumi.g, config);
  lumi.b = GetProperty_Float(lumiB_s, lumi.b, config);

  resetServer = GetProperty_Bool(resetServer_s, true, config);
  resetServerWaitMS = GetProperty_Int(resetServerWaitMS_s, 0, config);

  mode = static_cast<Mode>(GetProperty_Int(mode_s, static_cast<int>(Mode::PRIMARYDISPLAY), config));
  switch (mode)
  {
  case Mode::RESTART: runFunc = &RunServerRestartBroadcast; break;
  case Mode::SHUTDOWN: runFunc = &RunServerShutdownBroadcast; break;
  case Mode::CONFIG: runFunc = &RunServerConfigBroadcast; break;
  case Mode::BLACKOUT:  runFunc = &RunBlackoutBroadcast; break;
  case Mode::PRIMARYDISPLAY: case Mode::SPECIFICDISPLAY :  runFunc = &RunScreenCaptureBroadcast; break;
  case Mode::IMAGEFILE:  runFunc = &RunStaticImageBroadcast; break;
  case Mode::IMAGESEQUENCE:  runFunc = &RunImageSequenceAnimation; break;
  case Mode::SCRIPT:  runFunc = &RunScriptAnimation; break;
  default: cout << "WARNING! runFunc not set as invalid value was specified in config!" << endl << endl;
  }

  displayName = GetProperty_String(displayName_s, "", config);

  imageFile = GetProperty_String(imageFile_s, "", config);
  staticImageBroadcastSleepMS = GetProperty_Int(staticImageBroadcastSleepMS_s, 0, config);

  animationSteps = GetProperty_Int(animationSteps_s, 50, config);
  animationDelayMS = GetProperty_Int(animationDelayMS_s, 50, config);

  scriptFile = GetProperty_String(scriptFile_s, "", config);
  imageSequenceFile = GetProperty_String(imageSequenceFile_s, "", config);
}

//This function must be called as soon as the program starts!
inline void InitConfigVariables()
{
  InitConfigVariables_Debug();
  InitConfigVariables_General();
  CheckConfigValues();
}
