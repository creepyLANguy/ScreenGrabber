#include "InitConfigVariablesHelper.hpp"
#include "SocketHelpers.hpp"
#include "CoreLogic.hpp"


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
