#include "InitConfigVariables.hpp"
#include "CommandLineUtils.hpp"
#include "CoreLogic.hpp"
#include "InitSockets.hpp"

/*
MAINTAIN THIS SEQUENCE:
1. Immediately initialise the globally shared variables as per config.
2. Process commandline args and override current values found.
3. Set up your sockets as per config.
4. Send blackout packet to reset all LEDs. This step is kinda optional.
5. Send server config packet which contains led count and physical brightness. Also kinda optional but is a good idea.
6.1. If you do step 4, you'll need to wait a certain amount of time to give the servers a chance to reconfigure themselves.
7. Run the corresponding function for the configured capture mode. May loop indefinitely.
7. Clean up anything that needs cleaning up. May not be reached in some modes.
*/
int main(const int argc, char** argv)
{
  SetConsoleTitle(kApplicationName);

  InitConfigVariables();

  ProcessCommandLine(argc, argv);

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

  return 0;
}
