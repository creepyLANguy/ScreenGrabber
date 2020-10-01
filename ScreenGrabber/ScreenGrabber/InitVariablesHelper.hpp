#ifndef INITVARIABLESHELPER_HPP
#define INITVARIABLESHELPER_HPP

#include "ConfigHelpers.hpp"
#include "Debug.hpp"

//Debug variables
vector<KeyValPair> debug_config;
bool debug_fps_cmd;
bool debug_fps_ide;
bool debug_visual;
bool debug_payload;
bool debug_drawAllFrames;
bool debug_mockPayload;
bool debug_mockChunks;
int debug_blankVal;
NoiseType debug_noiseType;
float debug_blankRegionModifier;
int debug_reportTimeMS;

inline void InitDebugVariables()
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
  debug_blankRegionModifier = GetProperty_Float("blankRegionModifier", blankVal_default, debug_config);
  debug_reportTimeMS = GetProperty_Int("reportTimeMS", 1000, debug_config);
}


//Normal execution variables
vector<KeyValPair> config;
LEDsCollection leds;
Lumi lumi;
int sleepMS;
int chunkUpdateTimeoutMS;
int width, height;
int downscaler;
float borderSamplePercentage;
float lowerBuffer, upperBuffer, leftBuffer, rightBuffer;
int originPositionOffset;
float brightnessPercentage;
int whiteDiffThresh;
int outlierDiffThresh;
int whiteLuminanceThresh;
int colourLuminanceThresh;
bool optimiseTransmitWithDelta;
int deltaEThresh;
DeltaEType deltaEType;
double (*deltaEFunc)(const LAB&, const LAB&) = nullptr;

inline void InitNormalVariables()
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
  borderSamplePercentage = GetProperty_Float("borderSample", 0.1f, config);
  lowerBuffer = GetProperty_Float("lowerBuffer", 0.0f, config);
  upperBuffer = GetProperty_Float("upperBuffer", 0.0f, config);
  leftBuffer = GetProperty_Float("leftBuffer", 0.0f, config);
  rightBuffer = GetProperty_Float("rightBuffer", 0.0f, config);

  brightnessPercentage = GetProperty_Float("brightness", 1.0f, config);
  whiteDiffThresh = GetProperty_Float("whiteDiffThresh", 1.0f, config) * 255;
  outlierDiffThresh = GetProperty_Float("outlierDiffThresh", 1.0f, config) * 255;
  whiteLuminanceThresh = GetProperty_Float("whiteLuminanceThresh", 0.0f, config) * 255;
  colourLuminanceThresh = GetProperty_Float("colourLuminanceThresh", 0.0f, config) * 255;
  optimiseTransmitWithDelta = GetProperty_Bool("optimiseTransmitWithDelta", false, config);
  deltaEThresh = GetProperty_Int("deltaEThresh", 0, config);

  deltaEType = static_cast<DeltaEType>(GetProperty_Int("deltaEType", static_cast<int>(DeltaEType::CIE2000), config));
  switch (deltaEType)
  {
  case DeltaEType::CIE76:  deltaEFunc = &Calc76; break;
  case DeltaEType::CIE94:  deltaEFunc = &Calc94; break;
  case DeltaEType::CIE2000:  deltaEFunc = &Calc2000; break;
  }

  lumi.r = GetProperty_Float("lumiR", lumi.r, config);
  lumi.g = GetProperty_Float("lumiG", lumi.g, config);
  lumi.b = GetProperty_Float("lumiB", lumi.b, config);
}


inline void InitVariables()
{
  InitDebugVariables();
  InitNormalVariables();
}


#endif // INITVARIABLESHELPER_HPP
