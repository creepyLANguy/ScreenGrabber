#pragma once

#include "ConfigVariables.h"

#define WARN(Variable, Warning) Print((#Variable), Warning)


const char* cautioner = "WARNING:\r\n";
const char* lessThanZero = " is less than zero.\r\n";
const char* lessThanOrEqualToZero = " is less than or equal to zero.\r\n";
const char* equalToZero = " is equal to zero.\r\n";
const char* moreThanOrEqualToZero = " is more than or equal to zero.\r\n";
const char* moreThanZero = " is more than zero.\r\n";

inline void Print(const char* variableName, const char* warning)
{
  cout << cautioner << variableName << warning << endl;
}


inline void CheckConfigValues_General()
{
  //AL.
  //TODO
  
  //if (width <= 0)
  {
    WARN(width, lessThanOrEqualToZero);
  }

  cout << endl;
}


inline void CheckConfigValues_Debug()
{
  //AL.
  //TODO

  cout << endl;
}


inline void CheckConfigValues()
{
  CheckConfigValues_General();
  CheckConfigValues_Debug();
  cout << endl;
}