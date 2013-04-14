/* This file protected by faith_base_00 license
 * No part of this file could be copyed or changed without Kirill Berezin agreement
 * No part of this file could be used or viewed without Kirill Berezin agreement
 */

#include "../exceptions/exception.h"
#include "socket_connection.h"
#include "../3party/ax/tl4_low.h"

using namespace ed::com;
using namespace ax::tl4;
typedef ax::tl4::LOW_STATUSES low_status;

socket_connection::socket_connection( std::string addr, int port ) : connected(false)
{
  unsigned int ip;
  low_status s = low::GetIp(addr.c_str(), ip);
  throw_assert(s != CANT_RESOLVE_IP);
  throw_assert(s == SUCCESS);
  ConnectAttempt(ip, port);
}

socket_connection::socket_connection( int incoming_descriptor )
  : connected(true), desc(incoming_descriptor)
{
}

void socket_connection::ConnectAttempt()
{
  ConnectAttempt(0, 0); // cause they ignoring after first call
}

void socket_connection::ConnectAttempt( unsigned int ip, int port )
{
  if (connected)
    return;
  low_status s = low::Connect(desc, ip, (unsafe_word)port);
  throw_assert(s != LOW_CONNECTION_REFUSED);
  if (s == SUCCESS)
    connected = true;
}

void socket_connection::Notify( const ed::event_notification & )
{
  ConnectAttempt();
  todo(Notify);
}

int socket_connection::Incoming( )
{
  ConnectAttempt();
  todo(Incoming);
}

socket_connection::~socket_connection()
{
  if (desc != _TL4_NOT_SOCKET_)
    low::Close(desc);
}