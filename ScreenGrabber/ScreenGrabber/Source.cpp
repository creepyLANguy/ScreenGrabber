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
  4. Send server config packet which contains led count and physical brightness. Also kinda optional but is a good idea.
  4.1. If you do step 4, you'll need to wait a certain amount of time to give the servers a chance to reconfigure themselves. 
  5. Run the corresponding function for the configured capture mode. May loop indefinitely.
  6. Clean up anything that needs cleaning up. May not be reached in some modes. 
  */

  InitConfigVariables();

  SetupSockets(tempSockets, sockets);

  RunBlackoutBroadcast();

  if (resetServer == true)
  {
    RunServerConfigBroadcast();
  }

  if (mode != Mode::CONFIG)
  {
    runFunc();
  }  

  CleanUpDeviceContextStuffs();
}


int main(const int argc, char** argv)
{
  Run();

  return 0;
}
