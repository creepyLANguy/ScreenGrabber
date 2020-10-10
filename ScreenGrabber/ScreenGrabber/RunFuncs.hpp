#pragma once

#include "ScreenCaptureHelpers.hpp"
#include "StaticImageHelpers.hpp"
#include "ImageSequenceHelpers.hpp"
#include "AnimationScriptHelpers.hpp"

inline void RunScreenCaptureBroadcast()
{
  RunScreenCapture();
}

inline void RunStaticImageBroadcast()
{
  RunStaticImageFileCapture();
}

inline void RunImageSequenceAnimation()
{
  if (ReadImageSequence()) { RunImageSequence(); }
}

inline void RunScriptAnimation()
{
  if (ReadScript()) { RunScript(); }
}
