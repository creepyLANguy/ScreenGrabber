#pragma once

#include "CoreLogic.hpp"

inline void RunBlackout()
{
  cout << "Sending [i,0,0,0] to each LED." << endl;

  unsigned int payload = static_cast<int>(ReservedIndex::UPDATE_ALL_LEDS) << 24 | 0 << 16 | 0 << 8 | 0;
  BroadcastPayload(payload);
  
  cout << "All blackout packets sent." << endl << endl;
}
