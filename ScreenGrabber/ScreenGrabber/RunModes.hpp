#pragma once

#include "ScreenCaptureLogic.hpp"
#include "StaticImageLogic.hpp"
#include "ImageSequenceLogic.hpp"
#include "AnimationScriptLogic.hpp"

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
