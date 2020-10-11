#include "InitConfigVariablesHelper.hpp"
#include "SocketInitHelpers.hpp"


inline void InitialiseEssentials()
{  
  InitConfigVariables(); //Make sure this is done asap.
  SetupSockets(tempSockets, sockets);
}


int main(const int argc, char** argv)
{
  InitialiseEssentials();
  
  runFunc();

  CleanUpDeviceContextStuffs();

  return 0;
}
