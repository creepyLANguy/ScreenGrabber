#include "MySocket.h"
#include <fstream>

#include <iostream>

#pragma comment(lib,"ws2_32.lib")

typedef char raw_type;

MySocket::MySocket(const string& host, const int port)
{
  mHost = host;
  mPort = port;
  mSock = 0;
}

bool MySocket::Initialise()
{
  WSADATA wsa;
  WSAStartup(0x101, &wsa);

  mSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (mSock == NULL)
  {
    return false;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ResolveIP();
  addr.sin_port = htons(mPort);
  const int res = connect(mSock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
  if (res)
  {
    closesocket(mSock);
    mSock = 0;
    mHost[0] = '\0';
    return false;
  }

  mInitialised = true;

  return mInitialised;
}

unsigned long MySocket::ResolveIP()
{
  int i = static_cast<int>(mHost.length());
    
  for (; i > 0; --i)
  {
    if (!(mHost[i - 1] == '.' || mHost[i - 1] == 'x' || mHost[i - 1] == 'X' || mHost[i - 1] >= '0' && mHost[i - 1] <= '9'))
    {
      break;
    }
  }

  if (i)
  {
    hostent* ph = gethostbyname(mHost.c_str());
    if (ph)
    {
      return *reinterpret_cast<unsigned long*>(ph->h_addr_list[0]);
    }
    return INADDR_NONE;
  }

  return inet_addr(mHost.c_str());
}

int MySocket::Send(void* payload) const
{
  return send(mSock, static_cast<raw_type*>(payload), sizeof(payload), 0);
}

MySocket::~MySocket()
{
  WSACleanup();
}
