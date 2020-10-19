/*
Format is as follows :

sS
dD
R G B xN
...

where,
S is animationSteps, which overwrites the value specified in config.ini
D is animationDelayMS, which overwrites the value specified in config.ini
N is amount of times this hue will repeat

s d and x values are optional

eg:

s2
d40
0 0 0
0 0 0
0 0 0

is same as :

s2
d40
0 0 0 x3
*/

#pragma once

#include <fstream>
#include <iostream>
#include "ConfigVariables.h"
#include "ScriptUtils.hpp"


vector<RGB> nodes;


inline bool ReadScript()
{
  cout << "Opening script file : " << scriptFile << endl << endl;

  ifstream myFile;
  myFile.open(scriptFile);
  if (myFile.is_open() == false)
  {
    cout << "Failed to open script file : " << scriptFile << endl;
    return false;
  }

  string strLine;
  while (myFile.eof() == false)
  {
    getline(myFile, strLine);
    strLine = Trim(strLine);

    if (IsCommentOrEmpty(strLine)) { continue; }

    if (CheckForScriptOverWritesInLine(strLine)) { continue; }

    string r = strLine.substr(0, strLine.find(kScriptDelim));
    strLine = strLine.substr(strLine.find(r) + strlen(r.c_str()) + 1);
    string g = strLine.substr(0, strLine.find(kScriptDelim));
    strLine = strLine.substr(strLine.find(g) + strlen(g.c_str()) + 1);
    string b = strLine;

    int repeats = 1;
    if (strLine.find(kScriptAnimationRepeatDelim) != string::npos)
    {
      strLine = strLine.substr(strLine.find(kScriptAnimationRepeatDelim) + 1);
      repeats = strlen(strLine.c_str()) > 0 ? atoi(strLine.c_str()) : 1;
    }
    
    RGB rgb;
    rgb.r = atoi(r.c_str());
    rgb.g = atoi(g.c_str());
    rgb.b = atoi(b.c_str());

    for (int i = 0; i < repeats; ++i)
    {
      nodes.emplace_back(rgb);
    }

    cout << rgb.r << " " << rgb.g << " " << rgb.b << " x" << repeats << endl;
  }

  cout << endl;

  if (nodes.empty())
  {
    std::cout << "Script yielded no keyframes." << scriptFile << endl;
    cout << endl;
    return false;
  }

  cout << "Script file successfully parsed." << endl;
  cout << "Total keyframes: " << nodes.size() << endl;
  cout << endl;

  myFile.close();

  return true;
}


inline void BroadcastRGB(const int r, const int g, const int b)
{
  unsigned int payload = static_cast<int>(ReservedIndex::UPDATE_ALL_LEDS) << 24 | r << 16 | g << 8 | b;
  BroadcastPayload(payload);
}


inline void RunScript()
{
  if (animationSteps <= 0)
  {
    cout << "animationSteps specified problematic." << endl;
    cout << "Value : " << animationSteps << endl;
    cout << "Exiting Program." << endl;
    return;
  }

  auto it = nodes.begin();
  auto next = it + 1;
  if (next == nodes.end()) { next = nodes.begin(); }

  while (true)
  {
    double step = 0;
    double rd = next->r - it->r;
    double gd = next->g - it->g;
    double bd = next->b - it->b;
    double rds = rd / animationSteps;
    double gds = gd / animationSteps;
    double bds = bd / animationSteps;

    while (step < animationSteps)
    {
      int r = it->r + (rds * step);
      int g = it->g + (gds * step);
      int b = it->b + (bds * step);

      r *= brightnessPercentage;
      g *= brightnessPercentage;
      b *= brightnessPercentage;

      r += redShift;
      r = r < 0 ? 0 : r;
      r = r > 255 ? 255 : r;
      b += blueShift;
      b = b < 0 ? 0 : b;
      b = b > 255 ? 255 : b;
      g += greenShift;
      g = g < 0 ? 0 : g;
      g = g > 255 ? 255 : g;

      BroadcastRGB(r, g, b);
    
      cout << r << "\t|" << g << "\t|" << b << endl;

      if (animationDelayMS > 0)
      {
        if (debug_scriptAnimation_show == true)
        {
          Mat mat(debug_scriptAnimation_rows, debug_scriptAnimation_cols, imageType);
          mat = Scalar(b, g, r);
          imshow("scriptAnimationDebugView", mat);
          waitKey(animationDelayMS);
        }
        else
        {
          Sleep(animationDelayMS);
        }
      }

      ++step;
    }

    ++it;
    if (it == nodes.end()) { it = nodes.begin(); }
    ++next;
    if (next == nodes.end()) { next = nodes.begin(); }
  }
}
