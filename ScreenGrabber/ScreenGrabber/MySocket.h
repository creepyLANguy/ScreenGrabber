#ifndef __MYSOCKET_INCLUDED__
#define __MYSOCKET_INCLUDED__

#include <winsock.h>
#include <string>

using namespace std;

const string kDefaultAddress = "127.0.0.1";
const int kDefaultPort = 8888;

class MySocket
{

public:
  MySocket();
  MySocket(string host, int port);
  bool Initialise();
  int Send(void* payload) const;
  ~MySocket();

  string GetHost() { return mHost; }
  int GetPort() { return mPort; }
  bool IsInitialised() { return mInitialised; }

  string ToString() { return mHost + ":" + std::to_string(mPort); }

private:
  unsigned long ResolveIP();

private:
  string mHost = kDefaultAddress;
  int mPort = kDefaultPort;
  SOCKET mSock = 0;
  bool mInitialised = false;

};

#endif //__MYSOCKET_INCLUDED__