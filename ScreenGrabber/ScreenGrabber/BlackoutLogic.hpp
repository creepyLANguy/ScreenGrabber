#pragma once

#include "CoreLogic.hpp"

inline void RunBlackout()
{
  cout << "Sending [ReservedIndex::UPDATE_ALL_LEDS, 0,0,0]" << endl;

  unsigned int payload = static_cast<int>(ReservedIndex::UPDATE_ALL_LEDS) << 24 | 0 << 16 | 0 << 8 | 0;
  BroadcastPayload(payload);
  
  cout << "Blackout packet sent." << endl << endl;
}
