#pragma once

#include "Debug.hpp"
#include "GlobalDefinitions.h"
#include "DeltaE.hpp"

//Debug variables
vector<KeyValPair> debug_config;
bool debug_fps_cmd;
bool debug_fps_ide;
bool debug_visual;
bool debug_chunkData;
bool debug_payload;
bool debug_drawAllFrames;
bool debug_mockPayload;
bool debug_mockChunks;
int debug_reportTimeMS;
bool debug_scriptAnimation_show;
int debug_scriptAnimation_rows;
int debug_scriptAnimation_cols;
bool debug_isPreWindows10Version2004;


//Normal execution variables
vector<KeyValPair> config;
LEDsCollection leds;
Lumi lumi;
int sleepMS;
int chunkUpdateTimeoutMS;
int width, height;
int downscaler;
float borderSamplePercentage;
float lowerBufferPercentage, upperBufferPercentage, leftBufferPercentage, rightBufferPercentage;
int originPositionOffset;
int physicalStripBrightness;
float brightnessPercentage;
int whiteBrightnessModifier;
int redShift, blueShift, greenShift;
int whiteDiffThresh;
int outlierDiffThresh;
int whiteLuminanceThresh;
int colourLuminanceThresh;
bool optimiseTransmitWithDelta;
int deltaEThresh;
DeltaEType deltaEType;
Mode mode;
string displayName;
string imageFile;
int staticImageBroadcastSleepMS;
string imageSequenceFile;
string scriptFile;
int animationSteps;
int animationDelayMS;
bool resetServer;
int resetServerWaitMS;


//Actual strings used in config files and cmdline

const string debug_visual_s = "draw_visual";
const string debug_chunkData_s = "draw_chunkData";
const string debug_drawAllFrames_s = "drawAllFrames";
const string debug_fps_cmd_s = "debug_fps_cmd";
const string debug_fps_ide_s = "debug_fps_ide";
const string debug_payload_s = "print_payload";
const string debug_mockPayload_s = "mockPayload";
const string debug_mockChunks_s = "mockChunks";
const string debug_reportTimeMS_s = "reportTimeMS";
const string debug_scriptAnimation_show_s = "debug_scriptAnimation_show";
const string debug_scriptAnimation_cols_s = "debug_scriptAnimation_cols";
const string debug_scriptAnimation_rows_s = "debug_scriptAnimation_rows";
const string debug_isPreWindows10Version2004_s = "debug_isPreWindows10Version2004";

const string width_s = "ratioHorizontal";
const string height_s = "ratioVertical";
const string borderSamplePercentage_s = "borderSamplePercentage";
const string lowerBufferPercentage_s = "lowerBufferPercentage";
const string upperBufferPercentage_s = "upperBufferPercentage";
const string leftBufferPercentage_s = "leftBufferPercentage";
const string rightBufferPercentage_s = "rightBufferPercentage";
const string led_count_upper_s = "led_count_upper";
const string led_count_lower_s = "led_count_lower";
const string led_count_left_s = "led_count_left";
const string led_count_right_s = "led_count_right";
const string originPositionOffset_s = "originIndex";
const string physicalStripBrightness_s = "physicalStripBrightness";
const string brightnessPercentage_s = "brightnessPercentage";
const string downscaler_s = "downscale";
const string sleepMS_s = "sleepMS";
const string chunkUpdateTimeoutMS_s = "chunkUpdateTimeoutMS";
const string whiteBrightnessModifier_s = "whiteBrightnessModifier";
const string lumiR_s = "lumiR";
const string lumiG_s = "lumiG";
const string lumiB_s = "lumiB";
const string redShift_s = "redShift";
const string blueShift_s = "blueShift";
const string greenShift_s = "greenshift";
const string whiteDiffThresh_s = "whiteDiffThreshPercentage";
const string outlierDiffThresh_s = "outlierDiffThreshPercentage";
const string whiteLuminanceThresh_s = "whiteLuminanceThreshPercentage";
const string colourLuminanceThresh_s = "colourLuminanceThreshPercentage";
const string optimiseTransmitWithDelta_s = "optimiseTransmitWithDelta";
const string deltaEThresh_s = "deltaEThresh";
const string deltaEType_s = "deltaEType";
const string resetServer_s = "resetServer";
const string resetServerWaitMS_s = "resetServerWaitMS";
const string mode_s = "mode";
const string displayName_s = "displayName";
const string imageFile_s = "imageFile";
const string staticImageBroadcastSleepMS_s = "staticImageBroadcastSleepMS";
const string animationSteps_s = "animationSteps";
const string animationDelayMS_s = "animationDelayMS";
const string imageSequenceFile_s = "imageSequenceFile";
const string scriptFile_s = "scriptFile";
