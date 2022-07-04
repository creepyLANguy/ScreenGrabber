#pragma once

#include "ServerRestartLogic.hpp"
#include "ServerShutdownLogic.hpp"
#include "ServerConfigLogic.hpp"
#include "BlackoutLogic.hpp"
#include "ScreenCaptureLogic.hpp"
#include "StaticImageLogic.hpp"
#include "ImageSequenceLogic.hpp"
#include "AnimationScriptLogic.hpp"
#include "MultiMaskLogic.hpp"

inline void RunServerRestartBroadcast()
{
  RunServerRestart();
}

inline void RunServerShutdownBroadcast()
{
  RunServerShutdown();
}

inline void RunServerConfigBroadcast()
{
  RunServerConfig();
}

inline void RunBlackoutBroadcast()
{
  RunBlackout();
}

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

inline void RunMultiMaskBroadcast()
{
  if (ReadMultiMask()) { RunMultiMask(); }
}
