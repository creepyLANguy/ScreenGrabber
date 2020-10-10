#pragma once

#include "Defines.h"
#include <bitset>


const char* kDebugConfigFileName = "debug_config.ini";


auto zeroHourFPS = GetTickCount();
auto zeroHourInfo = GetTickCount();
unsigned int currentFrameCount = 0;
unsigned int lastDeterminedFramerate = 0;
unsigned int currentChunksUpdated = 0;
unsigned int lastChunksUpdatedTotal = 0;
unsigned int updateOccasions = 0;
unsigned int lastUpdateOccasionsTotal = 0;
inline void UpdateDebugTimer(const int reportTimeMS, const unsigned int updatedChunksCount)
{
  ++currentFrameCount;

  if (updatedChunksCount > 0)
  {
    ++updateOccasions;
    currentChunksUpdated += updatedChunksCount;
  }

  const auto now = GetTickCount();

  if (now - zeroHourFPS > 1000)
  {
    lastDeterminedFramerate = currentFrameCount;
    currentFrameCount = 0;
    zeroHourFPS = now;
  }

  if (now - zeroHourInfo > reportTimeMS)
  {
    lastChunksUpdatedTotal = currentChunksUpdated;
    lastUpdateOccasionsTotal = updateOccasions;
    currentChunksUpdated = 0;
    updateOccasions = 0;
    zeroHourInfo = now;
  }
}


inline void PrintFramerate(const bool cmd, const bool ide)
{
  const string str = "FPS:" + to_string(lastDeterminedFramerate) + "\r\n";
  if (cmd)
  {
    cout << "\r\n" << str.c_str() << "\r\n";
  }
  if (ide)
  {
    OutputDebugStringA(str.c_str());
  }
}




enum NoiseType : int
{
  NONE =      (1 << 0), 
  LOGO =      (1 << 1), 
  GREY =      (1 << 2),
  COLOUR =    (1 << 3),
  SHIFTER_1 = (1 << 4),
  SHIFTER_2 = (1 << 5),
  SHIFTER_3 = (1 << 6),
  CHUNKDATA = (1 << 7),
  BLUR =      (1 << 8),
  RANDOM =    (1 << 9),
  NOISETYPE_LAST = 10
};

enum NoiseApplicator
{
  STATIC,
  DYNAMIC,
  EITHER,
  NOISEAPPLICATOR_LAST
};

int shifter = 0;


constexpr int blankVal_default = 150;
constexpr float blankRegionModifier_default = 2.5f;
constexpr NoiseType noiseType_default = static_cast<NoiseType>(BLUR | LOGO);

struct LogoText
{
  bool hasBeenInitialised = false;

  const string text = "PROJECT LUNA";
  const Scalar textColour = Scalar(255, 255, 255);
  const HersheyFonts font = FONT_HERSHEY_SIMPLEX;
  const double scale = 1;
  const int thickness = 1;
  const LineTypes line = LINE_AA;
  const Size textRegion = getTextSize(text, font, scale, thickness, nullptr);

  int y_start = -1;
  int y_end = -1;
  int x_start = -1;
  int x_end = -1;
  int height = -1;
  int width = -1;
  Rect region = Rect(-1,-1,-1,-1);

  int text_x = -1;
  int text_y = -1;
  Point text_origin = Point(-1, -1);
};

LogoText logoText;

inline void InitLogoText(Mat& mat, const float leeway)
{
  logoText.y_start = mat.rows * leeway;
  logoText.y_end = mat.rows * (1 - leeway);
  logoText.x_start = mat.cols * leeway;
  logoText.x_end = mat.cols * (1 - leeway);
  logoText.height = logoText.y_end - logoText.y_start;
  logoText.width = logoText.x_end - logoText.x_start;
  logoText.region = Rect(logoText.x_start, logoText.y_start, logoText.width, logoText.height);

  logoText.text_x = (logoText.width - logoText.textRegion.width) / 2 + logoText.x_start;
  logoText.text_y = (logoText.height / 2) + (logoText.textRegion.height / 2) + logoText.y_start;
  logoText.text_origin = Point(logoText.text_x, logoText.text_y);

  logoText.hasBeenInitialised = true;
}


struct ChunkDataText
{
  bool hasBeenInitialised = false;

  const string text = "chunkDataText";
  const Scalar textColour = Scalar(255, 255, 255);
  const HersheyFonts font = FONT_HERSHEY_SIMPLEX;
  const double scale = 0.4;
  const int thickness = 1;
  const LineTypes line = LINE_AA;
  const Size textRegion = getTextSize(text, font, scale, thickness, nullptr);

  int y_start = -1;
  int y_end = -1;
  int x_start = -1;
  int x_end = -1;
  int height = -1;
  int width = -1;
  Rect region = Rect(-1,-1,-1,-1);

  int text_x = -1;
  int text_y = -1;
  Point text_origin = Point(-1, -1);
};

ChunkDataText chunkDataText;

inline void InitChunkDataText(Mat& mat, const float leeway)
{
  chunkDataText.y_start = mat.rows * leeway;
  chunkDataText.y_end = mat.rows * (1 - leeway);
  chunkDataText.x_start = mat.cols * leeway;
  chunkDataText.x_end = mat.cols * (1 - leeway);
  chunkDataText.height = chunkDataText.y_end - chunkDataText.y_start;
  chunkDataText.width = chunkDataText.x_end - chunkDataText.x_start;
  chunkDataText.region = Rect(chunkDataText.x_start, chunkDataText.y_start, chunkDataText.width, chunkDataText.height);

  chunkDataText.text_x = chunkDataText.x_start + 4;
  chunkDataText.text_y = chunkDataText.y_start + chunkDataText.height - 4;
  chunkDataText.text_origin = Point(chunkDataText.text_x, chunkDataText.text_y);

  chunkDataText.hasBeenInitialised = true;
}


struct NoiseRegion
{
  bool hasBeenInitialised = false;

  int y_start = -1;
  int y_end = -1;
  int x_start = -1;
  int x_end = -1;
  int height = -1;
  int width = -1;
  Rect region = Rect(-1, -1, -1, -1);
};

NoiseRegion noiseRegion;

inline void InitNoiseRegion(Mat& mat, const float leeway)
{
  noiseRegion.y_start = mat.rows * leeway;
  noiseRegion.x_start = mat.cols * leeway;
  noiseRegion.y_end = mat.rows * (1 - leeway);
  noiseRegion.x_end = mat.cols * (1 - leeway);
  noiseRegion.height = noiseRegion.y_end - noiseRegion.y_start;
  noiseRegion.width = noiseRegion.x_end - noiseRegion.x_start;

  noiseRegion.region = Rect(noiseRegion.x_start, noiseRegion.y_start, noiseRegion.width, noiseRegion.height);

  noiseRegion.hasBeenInitialised = true;
}


int marginCounter = 0;
const int marginLimit = 5;
int marginModifier = 1;
const char marginMarker = ' ';
string margin;
inline void IncrementMargin()
{
  marginCounter += marginModifier;
  if (marginCounter > marginLimit)
  {
    marginCounter = marginLimit;
    marginModifier = -1;
  }
  else if (marginCounter < 0)
  {
    marginCounter = 0;
    marginModifier = 1;
  }
  margin = string(marginCounter, marginMarker);
}


inline void PrintPayload(const unsigned int& payload)
{
  cout << margin << bitset<32>(payload) << "\r\n";
}

const char* sep = " | ";
inline void PrintChunk(const BorderChunk& chunk)
{
  cout << margin << chunk.index << sep << chunk.r << sep << chunk.g << sep << chunk.b << "\r\n\r\n";
}


inline void FillMatChunksWithAverageRGB(vector<BorderChunk>& borderChunks, Mat& mat)
{
  auto* pixelPtr = static_cast<uint8_t*>(mat.data);
  const int cn = mat.channels();

  for (const BorderChunk& chunk : borderChunks)
  {
    for (int x = chunk.x_start; x < chunk.x_end; ++x)
    {
      for (int y = chunk.y_start; y < chunk.y_end; ++y)
      {
        pixelPtr[y*mat.cols*cn + x*cn + 2] = chunk.r;
        pixelPtr[y*mat.cols*cn + x*cn + 1] = chunk.g;
        pixelPtr[y*mat.cols*cn + x*cn + 0] = chunk.b;
      }
    }
  }
}


inline void RemoveSkippedChunks(vector<BorderChunk>& borderChunks, vector<int>& skippedChunksIndexes)
{
  for (int i = borderChunks.size() - 1; i >= 0; --i)
  {
    for (int j = skippedChunksIndexes.size() - 1; j >= 0; --j)
    {
      if (borderChunks[i].index == skippedChunksIndexes[j])
      {
        borderChunks.erase(borderChunks.begin() + i);
        skippedChunksIndexes.erase(skippedChunksIndexes.begin() + j);
        break;
      }
    }
  }
}


inline void SetNoiseValues(int& shift1, int& shift2, int& shift3, const NoiseType noiseType)
{
  switch (noiseType)
  {
  case GREY:
    shift1 = shift2 = shift3 = rand()%120;
    break;
  case COLOUR:
    shift1 = rand();
    shift2 = rand();
    shift3 = rand();
    break;
  case SHIFTER_1:
    shift1 = ++shifter;
    shift2 = ++shifter*++shifter;
    shift3 = 100;
    break;
  case SHIFTER_2:
    shift1 = shifter;
    shift2 = shifter * 2;
    shift3 = shifter * 3;
    ++shifter;
    break;
  case SHIFTER_3:
    shift1 = ++shifter*++shifter;
    shift2 = ++shifter*++shifter;
    shift3 = ++shifter*++shifter;
    break;
  case NONE:
  default:
    shift1 = shift2 = shift3 = 0;
    break;
  }
}


inline void FillWithNoise(
  Mat& mat, 
  const int blankVal = 0,
  const float leeway = 0,
  const NoiseType noiseType = NONE, 
  const NoiseApplicator noiseApplicator = EITHER)
{
  auto* pixelPtr = static_cast<uint8_t*>(mat.data);
  const int cn = mat.channels();

  int shift1 = 0, shift2 = 0, shift3 = 0;

  for (int y = noiseRegion.y_start; y < noiseRegion.y_end; ++y)
  {
    if (noiseApplicator == DYNAMIC)
    {
      SetNoiseValues(shift1, shift2, shift3, noiseType);
    }

    for (int x = noiseRegion.x_start; x < noiseRegion.x_end; ++x)
    {
      if (noiseApplicator == STATIC)
      {
        SetNoiseValues(shift1, shift2, shift3, noiseType);
      }

      pixelPtr[y*mat.cols*cn + x*cn + 2] = blankVal + shift1;
      pixelPtr[y*mat.cols*cn + x*cn + 1] = blankVal + shift2;
      pixelPtr[y*mat.cols*cn + x*cn + 0] = blankVal + shift3;
    }
  }
}

double totalChunks = 0;
inline void GetChunkDataString(string& s)
{
  const string spacer = "     ";

  s += "FPS : ";
  if (lastDeterminedFramerate < 10) { s += "0"; }
  s += to_string(lastDeterminedFramerate);
  s += spacer;

  const unsigned int averageUpdates = (lastUpdateOccasionsTotal > 0) ? (lastChunksUpdatedTotal / lastUpdateOccasionsTotal) : 0;

  s += "UPF : ";
  if (averageUpdates < 10) { s += "0"; }
  s += to_string(averageUpdates);
  s += spacer;

  const int percentage = static_cast<int>(averageUpdates / totalChunks * 100);
  s += "%PF : ";
  if (percentage < 10) { s += "0"; }
  s += to_string(percentage);
  s += spacer;

  if (percentage > 0 && percentage < 100) { s += "|"; }
  const int dots = percentage / 8;
  for (int i = 0; i < dots; ++i)
  {
    s += "|";
  }
}

inline void WriteChunkDataToMat(Mat& mat)
{
  string s;
  GetChunkDataString(s);

  putText(mat,
    s,
    chunkDataText.text_origin,
    chunkDataText.font,
    chunkDataText.scale,
    chunkDataText.textColour,
    chunkDataText.thickness,
    chunkDataText.line
  );  
}


inline void Blur(Mat& mat)
{
  //GaussianBlur(mat(region), mat(region), Size(0, 0), 9); 
  blur(mat(noiseRegion.region), mat(noiseRegion.region), noiseRegion.region.size());
}


inline void DrawLogo(Mat& mat)
{
  putText(mat, 
    logoText.text, 
    logoText.text_origin, 
    logoText.font, 
    logoText.scale, 
    logoText.textColour, 
    logoText.thickness, 
    logoText.line
  );
}


inline void BlankMat(
  Mat& mat, 
  const int blankVal = 0,
  const float leeway = 0,
  const NoiseType noiseType = NONE, 
  const NoiseApplicator noiseApplicator = STATIC)
{
  FillWithNoise(mat, blankVal, leeway, noiseType, noiseApplicator);
}


inline void ShowVisualisation(
  Mat& mat, const float& leeway, 
  vector<BorderChunk>& borderChunks, 
  vector<int>& skippedChunksIndexes, 
  vector<BorderChunk>& previousChunks,
  const int blankVal = 0,
  NoiseType noiseType = NONE, 
  NoiseApplicator noiseApplicator = EITHER)
{
  FillMatChunksWithAverageRGB(previousChunks, mat);
  RemoveSkippedChunks(borderChunks, skippedChunksIndexes);
  FillMatChunksWithAverageRGB(borderChunks, mat);

  if (noiseRegion.hasBeenInitialised == false)
  {
    InitNoiseRegion(mat, leeway);
  }

  if ((noiseType & RANDOM) == RANDOM)
  {
    noiseType = static_cast<NoiseType>(1 << rand() % NOISETYPE_LAST);
  }

  if ((noiseType & BLUR) == BLUR)
  {    
    Blur(mat);
  }

  if ((noiseType & CHUNKDATA) == CHUNKDATA)
  {
    if (chunkDataText.hasBeenInitialised == false)
    {
      InitChunkDataText(mat, leeway);
      totalChunks = previousChunks.empty() ? borderChunks.size() : previousChunks.size();
    }
    WriteChunkDataToMat(mat);
  }

  if ((noiseType & LOGO) == LOGO)
  {
    if (logoText.hasBeenInitialised == false)
    {
      InitLogoText(mat, leeway);
    }
    DrawLogo(mat);
  }
  else
  {
    if (noiseApplicator == EITHER)
    {
      noiseApplicator = static_cast<NoiseApplicator>(rand() % NOISEAPPLICATOR_LAST);
    }
    BlankMat(mat, blankVal, leeway, noiseType, noiseApplicator);
  }
  
  const String windowName;
  namedWindow(windowName, WINDOW_NORMAL);
  imshow(windowName, mat);
  waitKeyEx(1);
}


inline void GetDebugPayload(unsigned int& payload, const int i)
{
  payload = 0;
  int r = 0;
  int g = 0;
  int b = 0;

  //Alternative indexes are a dimmer or brighter blue.
  i%2 == 0 ? b = 55 : b = 255;    

  payload = i << 24 | r << 16 | g << 8 | b;
}


inline void GetDebugChunk(BorderChunk& chunk)
{
  chunk.r = chunk.g = chunk.b = 0;

  //Alternative indexes are a dimmer or brighter blue.
  chunk.index%2 == 0 ? chunk.b = 55 : chunk.b = 255;    
}
