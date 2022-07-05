#pragma once
#include "ConfigUtils.hpp"
#include "ConfigVariables.h"


inline vector<string> args;


inline int Int(const string s)
{
  return atoi(s.c_str());
}


inline int GetIndexOfValue(const string key)
{
  for (size_t i = 0; i < args.size(); ++i)
  {
    if (StringsAreEqual(key, args[i]))
    {
      return ++i;
    }
  }

  return -1;
}


inline pair<int, string> RetrieveValue(const string s)
{
  string str;

  const auto index = GetIndexOfValue(s);
  if (index > 0)
  {
    str = args[index];
  }

  return make_pair(index, str);
}


inline void SetInt(int& v, const string s)
{
  const auto pair = RetrieveValue(s);
  v = pair.first > 0 ? Int(pair.second) : v;
}

inline void SetFloat(float& v, const string s)
{
  const auto pair = RetrieveValue(s);
  v = pair.first > 0 ? static_cast<float>(Int(pair.second)) / 100.0f : v;
}

inline void SetBool(bool& v, const string s)
{
  const auto pair = RetrieveValue(s);
  v = pair.first > 0 ? Int(pair.second) == 1 : v;
}

inline void SetString(string& v, const string s)
{
  const auto pair = RetrieveValue(s);
  v = pair.first > 0 ? pair.second : v;
}


inline void ProcessCommandLine(const int argc, char** argv)
{
  args = vector<string>(argv, argv + argc);

  //TODO
  //Do this sorta thing for all the variables...
  //(Haven't completed this yet as normal config files are perfect for current use cases)
  SetInt(width, width_s);
  SetInt(height, height_s);
}