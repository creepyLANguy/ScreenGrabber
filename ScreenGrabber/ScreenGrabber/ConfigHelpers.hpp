#pragma once

#include "Defines.h"
#include <fstream>
#include <iostream>

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


inline string Trim(const string& str)
{
  size_t first = str.find_first_not_of(' ');
  if (string::npos == first)
  {
    return str;
  }
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}


inline bool StringsAreEqual(string str1, string str2)
{
  return _strcmpi(str1.c_str(), str2.c_str()) == 0;
}


inline string GetProperty_String(const string propertyName, const string default_return, const vector<KeyValPair>& configBlob)
{
  for (const KeyValPair& kvp : configBlob)
  {
    if (StringsAreEqual(kvp.key, propertyName))
    {
      return kvp.val.c_str();
    }
  }

  return default_return;
}


inline int GetProperty_Int(const string propertyName, const int default_return, const vector<KeyValPair>& configBlob)
{
  for (const KeyValPair& kvp : configBlob)
  {
    if (StringsAreEqual(kvp.key, propertyName))
    {
      return atoi(kvp.val.c_str());
    }
  }

  return default_return;
}


inline float GetProperty_Float(const string propertyName, const float default_return, const vector<KeyValPair>& configBlob)
{
  return GetProperty_Int(propertyName, (default_return * 100), configBlob) / 100.0f;
}


inline float GetProperty_Bool(const string propertyName, const bool default_return, const vector<KeyValPair>& configBlob)
{
  return GetProperty_Int(propertyName, default_return, configBlob) == 1;
}


inline void PopulateConfigBlob(const string configFileName, vector<KeyValPair>& configBlob)
{
  cout << configFileName << endl;

  ifstream myFile;
  myFile.open(configFileName);
  if (myFile.is_open() == false) { return; }

  string strLine;
  while (myFile.eof() == false)
  {;
    getline(myFile, strLine);
    strLine = Trim(strLine);
    if (strLine.length() == 0 || strLine[0] == kConfigCommentDelim) { continue; }

    KeyValPair kvp;
    int delimPos = strLine.find_first_of(kConfigDelim);
    kvp.key = Trim(strLine.substr(0, delimPos));

    int endIndex = strLine.find_first_of(kConfigCommentDelim);
    endIndex = endIndex == -1 ? strLine.length() : endIndex;
    ++delimPos;
    kvp.val = Trim(strLine.substr(delimPos, endIndex-delimPos));

    configBlob.push_back(kvp);

    cout << kvp.val << "\t=\t" << kvp.key << endl;
  }

  cout << endl;

  myFile.close();
}
