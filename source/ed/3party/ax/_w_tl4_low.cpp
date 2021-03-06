/* This file protected by faith_base_00 license
 * No part of this file could be copyed or changed without Develop Project agreement
 * No part of this file could be used or viewed without Develop Project agreement
 */

#include "../../exceptions/exception.h"
#pragma comment(lib, "ws2_32.lib")

#include <stdlib.h>
#include <WinSock2.h>
#include <fcntl.h>
#include "tl4_low.h"

#include "../../exceptions/disconnected.h"

using namespace ax::tl4;

namespace
{

#if 1
  class WSA
  {
  public:
    int Success;
    WSA( void )
    {
      WSADATA Data;

      if (WSAStartup(MAKEWORD(1, 0), &Data) != 0)
        Success = 0;
      else
        Success = 1;
    }
    ~WSA( void )
    {
      WSACleanup();
    }
  } aaa;
#endif
  void CloseSocket( unsafe_dword &s )
  {
    shutdown(s, SD_BOTH);
    closesocket(s);
  }

  bool CreateSocket( unsafe_dword &s )
  {
    if (s != INVALID_SOCKET)
      return true;
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
      return false;
    unsigned long a = 1;
    if (ioctlsocket(s, FIONBIO, &a) == SOCKET_ERROR)
    {
      CloseSocket(s);
      return false;
    }
    return true;
  }

  LOW_STATUSES UnderstandTheError( int res = 0 )
  {
    if (!res)
      res = WSAGetLastError();
    if (res == WSAEISCONN)
      return SUCCESS;
    if (res == WSAEWOULDBLOCK || res == WSAEINVAL)
      return TRYING_CONNECT;
    if (res == WSAENOTSOCK)
      throw_message("Low level protect. Random socket descriptor");
    if (res == WSAECONNRESET || res == WSAECONNABORTED)
      throw ed::disconnected();
    todo("Reaction to other statuses");
    dead_space();
  }

  LOW_STATUSES UnderstandTheMessagesError( int r = 0 )
  {
    if (!r)
      r = WSAGetLastError();
    LOW_STATUSES res = UnderstandTheError(r);
    if (res == TRYING_CONNECT)
      return NO_MESSAGES;
    return res;
  }
}

LOW_STATUSES low::GetIp( const char * const addr, unsafe_dword &ip )
{
  hostent *a = gethostbyname(reinterpret_cast<const char *>(addr));
  if (a == NULL)
  {
    ip = _TL4_NOT_IP_;
    return CANT_RESOLVE_IP;
  }
  ip = *(dword *)a->h_addr_list[0];
  return SUCCESS;
}

LOW_STATUSES low::Connect( unsafe_dword &s, 
                          unsafe_dword addr, const unsafe_word port )
{
  if (!CreateSocket(s))
    return CANT_CREATE_SOCKET;
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.S_un.S_addr = addr;
  memset(address.sin_zero, 0, 8);
  if (connect(s, (sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    return UnderstandTheError();
  return SUCCESS;
}

LOW_STATUSES low::Send( const unsafe_dword s, const unsafe_byte *buff, const word size )
{
  if (s == _TL4_NOT_SOCKET_ || buff == NULL)
    throw_message("Low level protect");
  int a;
  if ((a = send(s, (char *)buff, size, 0)) == -1)
    return UnderstandTheMessagesError();

  throw_assert(a == size);
  return SUCCESS;
}

LOW_STATUSES low::Recieve( const unsafe_dword s, byte *const buff, word &readed, word size )
{
  if (s == _TL4_NOT_SOCKET_ || buff == NULL)
    throw_message("Low level protect");
  readed = 0;
  short res = recv(s, (char *)buff, size, 0);
  if (res == -1)
    return UnderstandTheMessagesError();
  if (res == 0)
    return DISCONNECT;

  readed = res;
  return SUCCESS;
}

inline int Incoming( const unsafe_dword s, word size )
{
  char buffer[_TL4_DATA_SEGMENT_SIZE];
  short res = recv(s, buffer, size, MSG_PEEK);
  if (res == -1)
  {
    LOW_STATUSES t = UnderstandTheMessagesError();
    if (t == NO_MESSAGES)
      return 0;
    if (t == DISCONNECT)
      return -1;
    dead_space();
  }
  if (res == 0)
    return -1;
  return res;
}

int low::Incoming( const unsafe_dword s, word size )
{
  if (s == _TL4_NOT_SOCKET_)
    throw_message("Low level protect");
  throw_assert(size <= _TL4_DATA_SEGMENT_SIZE);
  int status = ::Incoming(s, 1);
  if (status < 1)
    return status;
  do
  {
    int t = ::Incoming(s, size);
    if (!t)
      size >>= 1;
    else
      return t;
    if (size == 0)
      return status;
  } while (true);
  dead_space();
}


LOW_STATUSES low::Close( unsafe_dword &s )
{
  if (s == _TL4_NOT_SOCKET_)
    throw_message("Low level protect");
  shutdown(s, SD_BOTH);
  closesocket(s);
  s = _TL4_NOT_SOCKET_;
  return SUCCESS;
}

LOW_STATUSES low::Open( unsafe_dword &s, 
                       const unsafe_word port, const unsafe_word max_connections )
{
  if (!CreateSocket(s))
    return CANT_CREATE_SOCKET;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)))
  {
    UnderstandTheError();
    return CANT_BIND;
  }
  if (listen(s, 0))
    return CANT_LISTEN;
  Sleep(1);
  return SUCCESS;
}

udw low::Accept( const unsafe_dword s, unsafe_dword &ip, unsafe_word &port )
{
  struct sockaddr_in a;
  int size = sizeof(a);
  udw res = (udw)accept(s, (struct sockaddr *)&a, &size);
  if (res != INVALID_SOCKET)
  {
    ip = a.sin_addr.S_un.S_addr;
    port = ntohs(a.sin_port);
    return res;
  }
  else
    UnderstandTheError();
  ip = _TL4_NOT_IP_;
  port = _TL4_NOT_PORT_;
  return _TL4_NOT_SOCKET_;
}

LOW_STATUSES low::SetBacklog( const unsafe_dword socket, 
                             const unsafe_word max_connections )
{
  if (listen(socket, max_connections))
  {
    UnderstandTheError();
    return CANT_LISTEN;
  }
  return SUCCESS;
}