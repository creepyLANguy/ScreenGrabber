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
  bool Initialise();
  int Send(void* payload) const;
  ~MySocket();

private:
  void MySocket::ReadConfig(string& str, string filename) const;
  void MySocket::ReadConfig(int& val, string filename) const;
  unsigned long ResolveIP();

private:
  string mHost = kDefaultAddress;
  int mPort = kDefaultPort;
  SOCKET mSock = 0;

};

#endif //__MYSOCKET_INCLUDED__