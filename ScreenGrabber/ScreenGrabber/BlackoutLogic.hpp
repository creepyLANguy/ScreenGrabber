#pragma once

#include "ConfigVariables.h"
#include "CoreLogic.hpp"

inline void RunBlackout()
{
  cout << "Sending [i,0,0,0] to each LED." << endl;

  for (int i = 0; i < leds.LED_COUNT_TOTAL; ++i)
  {
    unsigned int payload = 0;
    payload = i << 24 | 0 << 16 | 0 << 8 | 0;

    BroadcastPayload(payload);
  }

  cout << "All blackout packets sent.\nExiting..." << endl;
}
