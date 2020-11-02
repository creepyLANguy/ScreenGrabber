#pragma once

#include "GlobalDefinitions.h"
#include <fstream>
#include <iostream>


inline bool IsCommentOrEmpty(const string& str)
{
  return str.length() == 0 || str[0] == kConfigCommentDelim;
}


inline string Trim(const string& str)
{
  const size_t first = str.find_first_not_of(' ');
  if (string::npos == first)
  {
    return str;
  }
  const size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}


inline bool StringsAreEqual(string str1, string str2)
{
  return _strcmpi(Trim(str1).c_str(), Trim(str2).c_str()) == 0;
}


inline string GetProperty_String(const string& propertyName, const string default_return, const vector<KeyValPair>& configBlob)
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


inline int GetProperty_Int(const string& propertyName, const int default_return, const vector<KeyValPair>& configBlob)
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


inline float GetProperty_Float(const string& propertyName, const float default_return, const vector<KeyValPair>& configBlob)
{
  return GetProperty_Int(propertyName, (default_return * 100), configBlob) / 100.0f;
}


inline float GetProperty_Bool(const string& propertyName, const bool default_return, const vector<KeyValPair>& configBlob)
{
  return GetProperty_Int(propertyName, default_return, configBlob) == 1;
}


inline void PopulateConfigBlob(const string& configFileName, vector<KeyValPair>& configBlob)
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

    if (IsCommentOrEmpty(strLine)) { continue; }

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


inline void ReadList_String(vector<string>& list, const string& filename)
{
  ifstream myFile_Server;
  myFile_Server.open(filename);
  if (myFile_Server.is_open())
  {
    string strLine;
    while (myFile_Server >> strLine)
    {
      strLine = Trim(strLine);

      if (IsCommentOrEmpty(strLine)) { continue; }

      list.emplace_back(strLine);
    }
  }
  myFile_Server.close();
}


inline void ReadList_Int(vector<int>& list, const string& filename)
{
  vector<string> stringList;
  ReadList_String(stringList, filename);
  for (const string& s : stringList)
  {
    list.emplace_back(atoi(s.c_str()));
  }
}