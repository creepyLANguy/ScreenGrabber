#ifndef ANIMATIONSCRIPTHELPERS_HPP
#define ANIMATIONSCRIPTHELPERS_HPP

#include <fstream>
#include <iostream>
#include "ConfigVariables.h"


vector<RGB> nodes;


inline bool ReadScript()
{
  std::cout << "Opening script file : " << scriptFile << endl << endl;

  ifstream myFile;
  myFile.open(scriptFile);
  if (myFile.is_open() == false)
  {
    std::cout << "Failed to open script file : " << scriptFile << endl;
    return false;
  }

  while (myFile.eof() == false)
  {
    string strLine = "";
    getline(myFile, strLine);
    if (strLine.length() == 0) { continue; }

    string r = strLine.substr(0, strLine.find(kScriptDelim));
    strLine = strLine.substr(strLine.find(r)+strlen(r.c_str())+strlen(kScriptDelim));
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
  if (scriptSteps <= 0)
  {
    cout << "scriptSteps specified problematic.\nValue : " << scriptSteps << endl;
    cout << "Exiting Program." << endl;
    return;
  }

  auto it = nodes.begin();
  auto next = it + 1;
  if (next == nodes.end()) { next = nodes.begin(); }

  while (true)
  {
    double step = 1;
    double rd = (*next).r - (*it).r;
    double gd = (*next).g - (*it).g;
    double bd = (*next).b - (*it).b;
    double rds = rd / scriptSteps;
    double gds = gd / scriptSteps;
    double bds = bd / scriptSteps;

    while (step <= scriptSteps)
    {
      int r = (*it).r + (rds * step);
      int g = (*it).g + (gds * step);
      int b = (*it).b + (bds * step);

      BroadcastRGB(r, g, b);
    
      cout << r << "\t|" << g << "\t|" << b << "\r\n";

      if (scriptDelayMS > 0)
      {
        if (debug_scriptAnimation == true)
        {
          Mat mat(250, 250, CV_8UC3);
          mat = Scalar(r, g, b);
          imshow("scriptAnimationDebugView", mat);
          waitKey(scriptDelayMS);
        }
        else
        {
          Sleep(scriptDelayMS);
        }
      }

      ++step;
    }

    step = 1;
    ++it;
    if (it == nodes.end()) { it = nodes.begin(); }
    ++next;
    if (next == nodes.end()) { next = nodes.begin(); }
  }
}


#endif //ANIMATIONSCRIPTHELPERS_HPP