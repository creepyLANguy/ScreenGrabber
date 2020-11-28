#pragma once

#include "ConfigVariables.h"
#include "CoreLogic.hpp"

inline void RunServerShutdown()
{
  cout << "Sending [ReservedIndex::SHUTDOWN_SERVER, 0, 0, 0]" << endl;

  unsigned int payload = static_cast<int>(ReservedIndex::SHUTDOWN_SERVER) << 24 | 0 << 16 | 0 << 8 | 0;
  BroadcastPayload(payload);

  cout << "Server restart request sent." << endl << endl;
}
