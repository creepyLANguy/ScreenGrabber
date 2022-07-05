#pragma once

#include "ConfigVariables.h"
#include "CoreLogic.hpp"

inline bool ReadMultiMask()
{
  cout << "Opening masks config file : " << masksFile << endl << endl;

  ifstream myFile;
  myFile.open(masksFile);
  if (myFile.is_open() == false)
  {
    cout << "Failed to open masks config file : " << masksFile << endl;
    return false;
  }

  string strLine;
  while (myFile.eof() == false)
  {
    getline(myFile, strLine);
    strLine = Trim(strLine);

    if (IsCommentOrEmpty(strLine)) { continue; }

    string r = strLine.substr(0, strLine.find(kScriptDelim));
    strLine = strLine.substr(strLine.find(r) + r.length() + 1);
    string g = strLine.substr(0, strLine.find(kScriptDelim));
    strLine = strLine.substr(strLine.find(g) + g.length() + 1);
    string b = strLine;

    RGB rgb;
    rgb.r = atoi(r.c_str());
    rgb.g = atoi(g.c_str());
    rgb.b = atoi(b.c_str());

    nodes.emplace_back(rgb);
    
    cout << rgb.r << " " << rgb.g << " " << rgb.b << endl;
  }

  cout << endl;

  if (nodes.empty())
  {
    cout << "Error : Masks config yielded no masks." << endl;
    cout << endl;
    return false;
  }

  cout << "Masks config file successfully parsed." << endl;
  cout << "Total masks: " << nodes.size() << endl;
  cout << endl;

  myFile.close();

  return true;
}

inline void RunMultiMask()
{
  //AL.
  //TODO
}