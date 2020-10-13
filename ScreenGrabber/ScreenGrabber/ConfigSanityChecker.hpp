#pragma once

#include "ConfigVariables.h"


string lessThanZero = " is less than zero.\r\n";
string lessThanOrEqualToZero = " is less than or equal to zero.\r\n";
string equalToZero = " is equal to zero.\r\n";
string moreThanOrEqualToZero = " is more than or equal to zero.\r\n";
string moreThanZero = " is more than zero.\r\n";

inline void PrintWarning(string variableName, string warning)
{
  cout << variableName << warning << endl;
}

inline void CheckConfigValues_General()
{
  if (width <= 0)
  {
    
  }

  cout << endl;
}


inline void CheckConfigValues_Debug()
{
  cout << endl;
}


inline void CheckConfigValues()
{
  CheckConfigValues_General();
  CheckConfigValues_Debug();
  cout << endl;
}