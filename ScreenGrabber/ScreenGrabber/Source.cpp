#include "InitConfigVariablesHelper.hpp"
#include "SocketHelpers.hpp"
#include "CoreLogic.hpp"


inline void InitialiseEssentials()
{  
  InitConfigVariables(); //Make sure this is done asap.
  SetupSockets(tempSockets, sockets);
}


auto main(const int argc, char** argv) -> int
{
  InitialiseEssentials();
  
  runFunc();

  CleanUpDeviceContextStuffs();

  return 0;
}
