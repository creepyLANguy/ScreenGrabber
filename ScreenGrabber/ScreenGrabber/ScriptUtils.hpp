#pragma once

#include "ConfigVariables.h"

inline bool CheckForScriptOverWritesInLine(const string& strLine)
{
  if (strLine[0] == kScriptAnimationStepsDelim)
  {
    animationSteps = atoi(strLine.substr(1).c_str());
    cout << "Updated animationSteps as per script." << endl;
    cout << "New Value: " << animationSteps << endl;
    cout << endl;
    return true;
  }

  if (strLine[0] == kScriptAnimationDelayDelim)
  {
    animationDelayMS = atoi(strLine.substr(1).c_str());
    cout << "Updated animationDelayMS as per script." << endl;
    cout << "New Value: " << animationDelayMS << endl;
    cout << endl;
    return true;
  }

  return false;
}
