#pragma once

#include "ConfigVariables.h"
#include "CoreLogic.hpp"

inline void RunServerConfig()
{
  cout << "Sending [ReservedIndex::CONFIGURE_SERVER, 0," << physicalStripBrightness << "," << leds.LED_COUNT_TOTAL << "]" << endl;

  unsigned int payload = static_cast<int>(ReservedIndex::CONFIGURE_SERVER) << 24 | 0 << 16 | physicalStripBrightness << 8 | leds.LED_COUNT_TOTAL;
  BroadcastPayload(payload);

  cout << "Server config sent." << endl << endl;
}
