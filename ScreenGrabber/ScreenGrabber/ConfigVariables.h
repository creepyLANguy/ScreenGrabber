#pragma once

#include "Debug.hpp"
#include "Defines.h"
#include "DeltaE.hpp"

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
bool debug_scriptAnimation_show;
int debug_scriptAnimation_rows;
int debug_scriptAnimation_cols;


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
int whiteBrightnessModifier;
int whiteDiffThresh;
int outlierDiffThresh;
int whiteLuminanceThresh;
int colourLuminanceThresh;
bool optimiseTransmitWithDelta;
int deltaEThresh;
DeltaEType deltaEType;
double (*deltaEFunc)(const LAB&, const LAB&) = nullptr;
CaptureType captureType;
string imageFile;
int staticImageBroadcastSleepMS;
string imageSequenceFile;
string scriptFile;
void (*runFunc)() = nullptr;
int animationSteps;
int animationDelayMS;
string animationDirectory;
