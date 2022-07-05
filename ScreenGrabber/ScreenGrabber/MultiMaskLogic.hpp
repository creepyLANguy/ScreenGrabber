#pragma once

#include "ConfigVariables.h"

vector<MaskGroup> maskGroups;

inline void SplitByDelim(string str, const char* delim, vector<string>& output)
{
  size_t pos = 0;
  while ((pos = str.find(delim)) != std::string::npos) 
  {
    string token = str.substr(0, pos);
    output.emplace_back(token);
    str.erase(0, pos + strlen(delim));
  }
}

inline void ExtractLedIndexes(const string& ledIndexes, const char* delim, const vector<int>& output)
{
  vector<string> splitIndexes;
  SplitByDelim(ledIndexes, kMaskGroupIndexDelim, splitIndexes);

  //AL.
  //TODO - convert each of the splitIndexes strings into the respective ints and add to output.
}

inline void ExtractMaskPixelRanges(const string& maskFilename, const vector<pixelRange>& output)
{
  //AL.
  //TODO - implement
}

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

  while (myFile.eof() == false)
  {
    string buff;

    getline(myFile, buff);
    string maskFilename = Trim(buff);
    if (IsCommentOrEmpty(maskFilename)) { continue; }

    getline(myFile, buff);
    string ledIndexes = Trim(buff);
    if (IsCommentOrEmpty(ledIndexes)) { continue; }

    MaskGroup maskGroup;
    maskGroup.maskFile = maskFilename;
    ExtractLedIndexes(ledIndexes, kMaskGroupIndexDelim, maskGroup.ledIndexes);
    ExtractMaskPixelRanges(maskFilename, maskGroup.pixelRanges);

    maskGroups.emplace_back(maskGroup);
  }

  myFile.close();

  if (maskGroups.empty())
  {
    cout << "Error : Masks config yielded no mask groups." << endl;
    cout << endl;

    return false;
  }

  cout << "Masks config file successfully parsed." << endl;
  cout << "Total mask groups: " << maskGroups.size() << endl;
  cout << endl;

  return true;
}

inline void RunMultiMask()
{
  //AL.
  //TODO
}