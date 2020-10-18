#pragma once

#include <winsock.h>
#include <string>

using namespace std;

class MySocket
{

public:
  MySocket() = default;
  MySocket(const string& host, int port);
  bool Initialise();
  int Send(void* payload) const;
  ~MySocket();

  string GetHost() const { return mHost; }
  int GetPort() const { return mPort; }
  bool IsInitialised() const { return mInitialised; }

  string ToString() { return mHost + ":" + to_string(mPort); }

private:
  unsigned long ResolveIP();

private:
  string mHost;
  int mPort;
  SOCKET mSock = 0;
  bool mInitialised = false;

};
