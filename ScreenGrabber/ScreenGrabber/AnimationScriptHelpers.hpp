#pragma once

#include <fstream>
#include <iostream>
#include "ConfigVariables.h"


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

  while (myFile.eof() == false)
  {
    string strLine;
    getline(myFile, strLine);
    if (strLine.length() == 0 || strLine[0] == kConfigCommentDelim) { continue; }

    string r = strLine.substr(0, strLine.find(kScriptDelim));
    strLine = strLine.substr(strLine.find(r) + strlen(r.c_str()) + strlen(kScriptDelim));
    string g = strLine.substr(0, strLine.find(kScriptDelim));
    strLine = strLine.substr(strLine.find(g) + strlen(g.c_str()) + strlen(kScriptDelim));
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
    std::cout << "Script yielded no keyframes." << scriptFile << endl;
    return false;
  }

  cout << "Script file successfully parsed." << endl;

  myFile.close();

  return true;
}


inline void BroadcastRGB(const int r, const int g, const int b)
{
  for (int i = 0; i < leds.LED_COUNT_TOTAL; ++i)
  {
    unsigned int payload = i << 24 | r << 16 | g << 8 | b;
    for (const MySocket& socket : sockets)
    {
      socket.Send(&payload);
    }
  }
}


inline void RunScript()
{
  if (animationSteps <= 0)
  {
    cout << "animationSteps specified problematic.\nValue : " << animationSteps << endl;
    cout << "Exiting Program." << endl;
    return;
  }

  auto it = nodes.begin();
  auto next = it + 1;
  if (next == nodes.end()) { next = nodes.begin(); }

  while (true)
  {
    double step = 0;
    double rd = (*next).r - (*it).r;
    double gd = (*next).g - (*it).g;
    double bd = (*next).b - (*it).b;
    double rds = rd / animationSteps;
    double gds = gd / animationSteps;
    double bds = bd / animationSteps;

    while (step <= animationSteps)
    {
      const int r = (*it).r + (rds * step);
      const int g = (*it).g + (gds * step);
      const int b = (*it).b + (bds * step);

      BroadcastRGB(r, g, b);
    
      cout << r << "\t|" << g << "\t|" << b << "\r\n";

      if (animationDelayMS > 0)
      {
        if (debug_scriptAnimation == true)
        {
          Mat mat(250, 250, imageType);
          mat = Scalar(r, g, b);
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
