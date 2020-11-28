#pragma once

#include "ConfigVariables.h"
#include "CoreLogic.hpp"

inline void RunServerRestart()
{
  cout << "Sending [ReservedIndex::RESTART_SERVER, 0, 0, 0]" << endl;

  unsigned int payload = static_cast<int>(ReservedIndex::RESTART_SERVER) << 24 | 0 << 16 | 0 << 8 | 0;
  BroadcastPayload(payload);

  cout << "Server restart request sent." << endl << endl;
}
