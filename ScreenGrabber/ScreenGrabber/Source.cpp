// ReSharper disable CppClangTidyCppcoreguidelinesNarrowingConversions
// ReSharper disable CppClangTidyClangDiagnosticUnknownEscapeSequence
// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
// ReSharper disable CppClangTidyBugproneNarrowingConversions
// ReSharper disable CppClangTidyClangDiagnosticSignCompare
// ReSharper disable CppClangTidyBugproneExceptionEscape
// ReSharper disable CppInconsistentNaming
// ReSharper disable IdentifierTypo
// ReSharper disable CppUseAuto


#include "InitConfigVariablesHelper.hpp"
#include "SocketHelpers.hpp"
#include "CoreLogic.hpp"


int main(const int argc, char** argv)
{
  //Very important to init the config blobs and variables immediately.
  InitConfigVariables();

  SetupSockets(tempSockets, sockets);

  runFunc();

  CleanUpDeviceContextStuffs();

  return 0;
}
