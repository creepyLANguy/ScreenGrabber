#pragma once

#include "GlobalDefinitions.h"
#include "MySocket.h"
#include "ConfigUtils.hpp"

const string baseError =
"Remember that each host needs its own port to be specified in the ports.ini file.\r\n"
"1) Close all instances of the application\r\n"
"2) Check your config files \r\n"
"3) Launch the application again. \r\n\r\n";

inline void ShowError(const string& s)
{
  cout << s << baseError << endl;
}

inline void ShowError(MySocket& socket)
{
  cout << "FAILED TO CREATE SOCKET: " << socket.ToString() << endl;
  cout << baseError << endl;
}

inline void ShowSuccess(MySocket& socket)
{
  cout << "Socket created: " << socket.ToString() << endl;
}


inline bool PopulateSocketList(vector<MySocket>& sockets)
{
  vector<string> hosts;
  ReadList_String(hosts, kHostsFileName);

  vector<int> ports;
  ReadList_Int(ports, kPortsFileName);

  if (hosts.size() == 0)
  {
    ShowError("HOSTS LIST IS EMPTY!");
    return false;
  }
  if (ports.size() == 0)
  {
    ShowError("PORTS LIST IS EMPTY!");
    return false;
  }
  if (hosts.size() != ports.size())
  {
    ShowError("WARNING!!!\r\nHOSTS LIST AND PORTS LIST ARE DIFFERENT LENGTHS!");
  }

  //Go by shorter max between hosts list and ports list to at least get some potentially usable ports created. 
  const int len = MIN(hosts.size(), ports.size());
  for (int i = 0; i < len; ++i)
  {
    sockets.emplace_back(MySocket(hosts[i], ports[i]));
  }

  return true;
}


inline void InitialiseSockets(vector<MySocket>& sockets)
{
  string failures;
  for (MySocket& socket : sockets)
  {
    if (socket.Initialise())
    {
      ShowSuccess(socket);
    }
    else
    {
      failures += (socket.ToString() + "\r\n");
    }
  }

  if (failures.length() > 0)
  {
    ShowError("Failed to init the following sockets:\r\n" + failures);
  }

  cout << "\r\n";
}


inline void GetFinalSocketList(vector<MySocket>& sockets, vector<MySocket>& finalSockets)
{
  for (MySocket& socket : sockets)
  {
    if (socket.IsInitialised())
    {
      finalSockets.emplace_back(socket);
    }
  }
}


inline void SetupSockets(vector<MySocket>& socketsTemp, vector<MySocket>& finalSockets)
{
  PopulateSocketList(socketsTemp);
  InitialiseSockets(socketsTemp);
  GetFinalSocketList(socketsTemp, finalSockets);
}
