#include "InitConfigVariables.hpp"
#include "CoreLogic.hpp"
#include "InitSockets.hpp"

void Run()
{
  /*
  MAINTAIN THIS SEQUENCE
  1. Immediately initialise the globally shared variables as per config.
  2. Set up your sockets as per config.
  3. Send blackout packet to reset all LEDs. This step is kinda optional. 
  4. Send server config packet which contains led count and physical brightness. Not strictly necessary but good to do.
  5. Run the corresponding function for the configured capture mode. May loop indefinitely.
  6. Clean up anything that needs cleaning up. May not be reached in some modes. 
  */

  InitConfigVariables();

  SetupSockets(tempSockets, sockets);

  RunBlackoutBroadcast();

  RunServerConfigBroadcast();

  runFunc();

  CleanUpDeviceContextStuffs();
}


int main(const int argc, char** argv)
{
  Run();

  return 0;
}
