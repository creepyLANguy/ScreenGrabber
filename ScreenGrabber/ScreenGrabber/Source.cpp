#include "InitConfigVariables.hpp"
#include "CoreLogic.hpp"
#include "InitSockets.hpp"

void Run()
{
  /*
  MAINTAIN THIS SEQUENCE
  1. Immediately initialise the globally shared variables as per config.
  2. Set up your sockets as per config.
  3. Send blackout packet to reset all LEDs. This step is optional. 
  4. Run the corresponding function for the configured capture mode. May loop indefinitely.
  5. Clean up anything that needs cleaning up. May not be reached in some modes. 
  */

  InitConfigVariables();

  SetupSockets(tempSockets, sockets);

  RunBlackoutBroadcast();

  runFunc();

  CleanUpDeviceContextStuffs();
}


int main(const int argc, char** argv)
{
  Run();

  return 0;
}
