#ifndef DEBUG_HPP
#define DEBUG_HPP

#define SECOND_MS 1000 
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
  uint8_t* pixelPtr = static_cast<uint8_t*>(mat.data);
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
  Mat& mat, const float leeway,
  const int blankVal, 
  const NoiseType noiseType, 
  const NoiseApplicator noiseApplicator)
{
  uint8_t* pixelPtr = static_cast<uint8_t*>(mat.data);
  const int cn = mat.channels();

  int shift1, shift2, shift3;

  for (int y = mat.rows*leeway; y < mat.rows*(1 - leeway); ++y)
  {
    if (noiseApplicator == OUTER)
    {
      SetNoiseValues(shift1, shift2, shift3, noiseType);
    }

    for (int x = mat.cols*leeway; x < mat.cols*(1 - leeway); ++x)
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

inline void BlankMat(
  Mat& mat, 
  const float leeway = 0, 
  const int blankVal = 0, 
  const NoiseType noiseType = NONE, 
  const NoiseApplicator noiseApplicator = INNER)
{
  if (noiseType == INCEPTION) { return; }

  if (noiseType == LOGO)
  {
    //AL.
    return;
  }

  FillWithNoise(mat, leeway, blankVal, noiseType, noiseApplicator);
}

inline void ShowVisualisation(Mat& mat, const float& borderSamplePercentage, vector<BorderChunk>& borderChunks)
{
  const float leeway = borderSamplePercentage * 2.5;//0;
  const int blankVal = 150;
  const NoiseType noiseType = LOGO;//static_cast<NoiseType>(rand() % NOISETYPE_LAST);
  const NoiseApplicator noiseApplicator = INNER;//static_cast<NoiseApplicator>(rand() % NOISEAPPLICATOR_LAST);
  BlankMat(mat, leeway, blankVal, noiseType, noiseApplicator);
  FillMatChunksWithAverageRGB(borderChunks, mat);
  const String windowName = "";
  namedWindow(windowName, CV_WINDOW_NORMAL);
  imshow(windowName, mat);
  waitKeyEx(1);
}

#endif // DEBUG_HPP
