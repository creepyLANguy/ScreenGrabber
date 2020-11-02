#pragma once

inline int Int(const string s)
{
  return atoi(s.c_str());
}

inline float Float(const string s)
{
  return Int(s) / 100.0f;
}

inline float Bool(const string s)
{
  return Int(s) == 1;
}


inline int GetIndexOfValue(const string key, vector<string> v)
{
  for (int i = 0; i < v.size(); ++i)
  {
    if (StringsAreEqual(key, v[i]))
    {
      return i + 1;
    }
  }

  return - 1;
}


inline void ProcessCommandLine(const int argc, char** argv)
{
  vector<string> v(argv, argv + argc);

  int index = 0;

  //AL.
  //TODO
  //Do this sorta thing for all the variables...
  //Alternatively do a nice map-based solution. But... effort... 
  index = GetIndexOfValue(width_s, v);
  if (index > 0) { width = Int(v[index]); }
  //
}