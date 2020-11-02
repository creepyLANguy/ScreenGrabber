#pragma once

inline int GetIndexOfValue(const string key, const vector<string>& args)
{
  for (int i = 0; i < args.size(); ++i)
  {
    if (StringsAreEqual(key, args[i]))
    {
      return i + 1;
    }
  }

  return -1;
}


inline int Int(const string s)
{
  return atoi(s.c_str());
}

inline float Float(const string s)
{
  return Int(s) / 100.0f;
}

inline bool Bool(const string s)
{
  return Int(s) == 1;
}


inline void SetInt(int& v, const string s, const vector<string>& args)
{
  const auto index = GetIndexOfValue(s, args);
  if (index > 0)
  {
    v = Int(args[index]);
  }
}

inline void SetFloat(float& v, const string s, const vector<string>& args)
{
  const auto index = GetIndexOfValue(s, args);
  if (index > 0)
  {
    v = Float(args[index]);
  }
}

inline void SetBool(bool& v, const string s, const vector<string>& args)
{
  const auto index = GetIndexOfValue(s, args);
  if (index > 0)
  {
    v = Bool(args[index]);
  }
}

inline void SeString(string& v, const string s, const vector<string>& args)
{
  const auto index = GetIndexOfValue(s, args);
  if (index > 0)
  {
    v = args[index];
  }
}


inline void ProcessCommandLine(const int argc, char** argv)
{
  vector<string> args(argv, argv + argc);

  //AL.
  //TODO
  //Do this sorta thing for all the variables...
  SetInt(width, width_s, args);
  SetInt(height, height_s, args);
  //
}