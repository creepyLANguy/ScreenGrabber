#ifndef DEBUG_HPP
#define DEBUG_HPP


enum NoiseType
{
  NONE,
  LOGO,
  GREY,
  COLOUR,
  SHIFTER_1,
  SHIFTER_2,
  SHIFTER_3,
  INCEPTION,
  CHUNKDATA,
  RANDOM,
  NOISETYPE_LAST
};


enum NoiseApplicator
{
  INNER,
  OUTER,
  EITHER,
  NOISEAPPLICATOR_LAST
};


int shifter = 0;
const string logoFilename = "logo_small.bmp";


#include <bitset>
inline void PrintPayload(const unsigned int& payload)
{
  cout << bitset<32>(payload) << "\r\n";
}


const char* sep = " | ";
inline void PrintChunk(const BorderChunk& chunk)
{
  cout << chunk.index << sep << chunk.r << sep << chunk.g << sep << chunk.b << "\r\n\r\n";
}


constexpr auto SECOND_MS = 1000;
DWORD zeroHour = GetTickCount();
unsigned int frameCount = 0;
inline void PrintFramerate()
{
  ++frameCount;
  if (GetTickCount() - zeroHour > SECOND_MS)
  {
    const string str = "FPS:" + to_string(frameCount) + "\r\n";
    OutputDebugStringA(str.c_str());
    zeroHour = GetTickCount();
    frameCount = 0;
  }
}


inline void FillMatChunksWithAverageRGB(vector<BorderChunk>& borderChunks, Mat& mat)
{
  auto* pixelPtr = static_cast<uint8_t*>(mat.data);
  const int cn = mat.channels();

  for (const BorderChunk chunk : borderChunks)
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
    shift1 = shift2 = shift3 = rand();
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
    shift1 = ++shifter*shifter;
    shift2 = ++shifter*shifter;
    shift3 = ++shifter*shifter;
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

  const int y_start = mat.rows * leeway;
  const int y_end = mat.rows * (1 - leeway);
  const int x_start = mat.cols * leeway;
  const int x_end = mat.cols * (1 - leeway);

  for (int y = y_start; y < y_end; ++y)
  {
    if (noiseApplicator == OUTER)
    {
      SetNoiseValues(shift1, shift2, shift3, noiseType);
    }

    for (int x = x_start; x < x_end; ++x)
    {
      if (noiseApplicator == INNER)
      {
        SetNoiseValues(shift1, shift2, shift3, noiseType);
      }

      pixelPtr[y*mat.cols*cn + x*cn + 2] = blankVal + shift1;
      pixelPtr[y*mat.cols*cn + x*cn + 1] = blankVal + shift2;
      pixelPtr[y*mat.cols*cn + x*cn + 0] = blankVal + shift3;
    }
  }
}

#include <opencv2/opencv.hpp>
inline void DrawLogo(Mat& mat, const float leeway)
{
  //AL.
  FillWithNoise(mat, 150, leeway);

  ///*
  const int y_start = mat.rows * leeway;
  const int y_end = mat.rows * (1 - leeway);
  const int x_start = mat.cols * leeway;
  const int x_end = mat.cols * (1 - leeway);
  const int width = x_end - x_start;
  const int height = y_end - y_start;

  const Mat logo = imread(logoFilename, CV_8UC4);
  Mat scaledLogo(width, height, logo.type());
  const float fx = static_cast<float>(scaledLogo.cols) / logo.cols;
  const float fy = static_cast<float>(scaledLogo.rows) / logo.rows;
  resize(logo, scaledLogo, scaledLogo.size(), fx, fy, INTER_AREA);

  auto* pixelMat = static_cast<uint8_t*>(mat.data);
  const int cnMat = mat.channels();

  auto* pixelLogo = static_cast<uint8_t*>(logo.data);
  const int cnLogo = logo.channels();

  for (int y = y_start; y < y_end; ++y)
  {
    for (int x = x_start; x < x_end; ++x)
    {
      pixelMat[y * mat.cols * cnMat + x * cnMat + 3] = pixelLogo[(y - y_start) * logo.cols * cnLogo + (x - x_start) * cnLogo + 3];
      pixelMat[y * mat.cols * cnMat + x * cnMat + 2] = pixelLogo[(y - y_start) * logo.cols * cnLogo + (x - x_start) * cnLogo + 2];
      pixelMat[y * mat.cols * cnMat + x * cnMat + 1] = pixelLogo[(y - y_start) * logo.cols * cnLogo + (x - x_start) * cnLogo + 1];
      pixelMat[y * mat.cols * cnMat + x * cnMat + 0] = pixelLogo[(y - y_start) * logo.cols * cnLogo + (x - x_start) * cnLogo + 0];
    }
  }
  //*/
}


inline void BlankMat(
  Mat& mat, 
  const int blankVal = 0,
  const float leeway = 0,
  const NoiseType noiseType = NONE, 
  const NoiseApplicator noiseApplicator = INNER)
{
  FillWithNoise(mat, blankVal, leeway, noiseType, noiseApplicator);
}

//#include <opencv2/opencv.hpp>
//inline void WriteChunkDataToMat(const Mat& mat, const vector<BorderChunk>& borderChunks, const vector<BorderChunk>& previousChunks)
//{
//  const int arrSize = previousChunks.size();
//  bool* indexTracker = new bool[arrSize];
//  memset(indexTracker, false, arrSize);
//  vector<string> strings;
//
//  for (BorderChunk chunk : borderChunks)
//  {
//    const string s = to_string(chunk.index) + " : " + to_string(chunk.r) + ", " + to_string(chunk.g) + ", " + to_string(chunk.b) + ", ";
//    strings.emplace_back(s);
//    indexTracker[chunk.index] = true;
//  }
//
//  for (int i = 0; i < borderChunks.size(); ++i)
//  {
//    if (indexTracker[i] == false)
//    {
//      BorderChunk chunk = previousChunks[i];
//      const string s = to_string(chunk.index) + " : " + to_string(chunk.r) + ", " + to_string(chunk.g) + ", " + to_string(chunk.b) + ", ";
//      strings.emplace_back(s);
//    }
//  }
//
//  Point org = Point(mat.cols / 2, mat.rows / 2);
//
//  for(string s : strings)
//  {
//    org.x += 10;
//    org.y += 10;
//    putText(mat, s, org, FONT_HERSHEY_SIMPLEX, 1, (125, 100, 55), 1, LINE_AA);
//  }
//  delete[] indexTracker;
//}


inline void ShowVisualisation(Mat& mat, const float& borderSamplePercentage, 
                              vector<BorderChunk>& borderChunks, vector<int>& skippedChunksIndexes, vector<BorderChunk>& previousChunks,
                              NoiseType noiseType = NONE, NoiseApplicator noiseApplicator = EITHER, const int blankVal = 150)
{
  FillMatChunksWithAverageRGB(previousChunks, mat);
  RemoveSkippedChunks(borderChunks, skippedChunksIndexes);
  FillMatChunksWithAverageRGB(borderChunks, mat);

  const float leeway = borderSamplePercentage * 2.5f;//0;

  if (noiseType == RANDOM) { noiseType = static_cast<NoiseType>(rand() % NOISETYPE_LAST); }
  if (noiseApplicator == EITHER) { noiseApplicator = static_cast<NoiseApplicator>(rand() % NOISEAPPLICATOR_LAST); }

  if (noiseType == CHUNKDATA)
  {
    BlankMat(mat, 255, leeway);
    //TODO
    //Draw the chunk data to the blanked area.
    //WriteChunkDataToMat(mat, borderChunks, previousChunks);
  }
  else if (noiseType == LOGO)
  {
    DrawLogo(mat, leeway);
  }
  else if (noiseType != INCEPTION)
  {
    BlankMat(mat, blankVal, leeway, noiseType, noiseApplicator);
  }

  const String windowName = "";
  namedWindow(windowName, WINDOW_NORMAL);
  imshow(windowName, mat);
  waitKeyEx(1);
}


inline void GetDebugPayload(unsigned int& payload, int i = -1, int r = -1, int g = -1, int b = -1)
{
  if (i >= 0)
  {
    if (i%2 == 0)
    {
      r = 0;
      g = 0;
      b = 50;
      payload = i << 24 | r << 16 | g << 8 | b;
    }
    else
    {
      r = 0;
      g = 0;
      b = 255;
    }
  }

  payload = i << 24 | r << 16 | g << 8 | b;
}

#endif // DEBUG_HPP
