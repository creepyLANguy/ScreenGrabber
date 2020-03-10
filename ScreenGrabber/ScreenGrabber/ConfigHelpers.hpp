#ifndef CONFIGHELPERS_HPP
#define CONFIGHELPERS_HPP

#include "Defines.h"
#include <fstream>

/*
int GetCommandLineProperty_Int(const int argc, char** argv, char* propertyName, const int default_return)
{
for (int i = 0; i < argc; ++i)
{
if (!_strcmpi(propertyName, argv[i]))
{
return atoi(argv[++i]);
}
}

return default_return;
}


float GetCommandLineProperty_Float(const int argc, char** argv, char* propertyName, const float default_return)
{
return GetCommandLineProperty_Int(argc, argv, propertyName, (default_return *100)) / 100.0f;
}


char* GetCommandLineProperty_String(const int argc, char** argv, char* propertyName, char* default_return)
{
for (int i = 0; i < argc; ++i)
{
if (!_strcmpi(propertyName, argv[i]))
{
return argv[++i];
}
}

return default_return;
}


/*
const char* GetProperty_String(const char* propertyName, const char* default_return, vector<KeyValPair>& configBlob)
{
for (KeyValPair kvp : configBlob)
{
if (_strcmpi(kvp.key.c_str(), propertyName) == 0)
{
return kvp.val.c_str();
}
}

return default_return;
}
*/

inline int GetProperty_Int(const char* propertyName, const int default_return, vector<KeyValPair>& configBlob)
{
  for (KeyValPair kvp : configBlob)
  {
    if (_strcmpi(kvp.key.c_str(), propertyName) == 0)
    {
      return atoi(kvp.val.c_str());
    }
  }

  return default_return;
}


inline float GetProperty_Float(const char* propertyName, const float default_return, vector<KeyValPair>& configBlob)
{
  return GetProperty_Int(propertyName, (default_return * 100), configBlob) / 100.0f;
}


inline void PopulateConfigBlob(vector<KeyValPair>& configBlob)
{
  ifstream myFile;
  myFile.open(kConfigFileName);
  if (myFile.is_open())
  {
    while (myFile.eof() == false)
    {
      string strLine = "";
      getline(myFile, strLine);
      const int delimPos = strLine.find_first_of(kDelim);
      KeyValPair kvp;
      kvp.key = strLine.substr(0, delimPos); ;
      kvp.val = strLine.substr(delimPos + 1, strLine.length());
      configBlob.push_back(kvp);
    }
  }
  myFile.close();
}

#endif // CONFIGHELPERS_HPP
