#include "InitConfigVariables.hpp"
#include "CoreLogic.hpp"
#include "InitSockets.hpp"

void Run()
{
  InitConfigVariables(); //Must be called immediately

  SetupSockets(tempSockets, sockets);

  runFunc();

  CleanUpDeviceContextStuffs();
}

int main(const int argc, char** argv)
{
  Run();

  return 0;
}
