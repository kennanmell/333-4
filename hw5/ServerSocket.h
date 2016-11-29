/*
 * Copyright 2012 Steven Gribble
 * Modified 2016 John Zahorjan
 *
 *  This file is part of the UW CSE 333 course project sequence
 *  (333proj).
 *
 *  333proj is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  333proj is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with 333proj.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HW5_SERVERSOCKET_H_
#define _HW5_SERVERSOCKET_H_

#include <netdb.h>       // for AF_UNSPEC, AF_INET, AF_INET6
#include <stdint.h>      // for uint16_t, etc.
#include <sys/types.h>   // for AF_UNSPEC, AF_INET, AF_INET6
#include <sys/socket.h>  // for AF_UNSPEC, AF_INET, AF_INET6
#include <string>        // for std::string

#include "CSE333Socket.h"

namespace hw5_net {

// A ServerSocket class abstracts away the messy details of creating a
// TCP listening socket at a specific port and on a (hopefully)
// externally visible IP address.  As well, a ServerSocket helps
// customers accept incoming client connections on the listening
// socket.
  class ServerSocket : public CSE333Socket {
 public:
  // This constructor creates a new ServerSocket object and associates
  // it with the provided port number.  
  // If the argument port == 0, a randomly chosen port is used.
  explicit ServerSocket(uint16_t port);

  // The destructor closes the sockets.
  virtual ~ServerSocket();

  // This function causes the ServerSocket to attempt to create a
  // listening socket and to bind it to the given port number on
  // whatever IP address the host OS recommends for us.  The caller
  // provides:
  //
  // - ai_family: whether to create an IPv4, IPv6, or "either" listening
  //   socket.  To specify IPv4, customers pass in AF_INET.  To specify
  //   IPv6, customers pass in AF_INET6.  To specify "either" (which
  //   leaves it up to BindAndListen() to pick, in which case it will
  //   typically try IPv4 first), customers pass in AF_UNSPEC.
  //
  // On failure this function returns false.  On success, it returns
  // true, sets socketFD_ to be the file descriptor for the
  // listening socket, and also returns (via an output parameter):
  //
  // - listen_fd: the file descriptor for the listening socket.
  void BindAndListen(int ai_family, int *listen_fd);

  // This function causes the ServerSocket to attempt to accept
  // an incoming connection from a client.  On failure, throws a
  // string exception.  If interrrupted by a signal, returns false;
  // On success, it returns true, and also returns (via output
  // parameters) the following:
  //
  // - accepted_fd: the file descriptor for the new client connection.
  //   The customer is responsible for close()'ing this socket when it
  //   is done with it.
  //
  // - client_addr: a C++ string object containing a printable
  //   representation of the IP address the client connected from.
  //
  // - client_port: a uint16_t containing the port number the client
  //   connected from.
  //
  // - client_dnsname: a C++ string object containing the DNS name
  //   of the client.
  //
  // - server_addr: a C++ string object containing a printable
  //   representation of the server IP address for the connection.
  //
  // - server_dnsname: a C++ string object containing the DNS name
  //   of the server.
  bool Accept(int *accepted_fd,
              std::string *client_addr, uint16_t *client_port,
              std::string *client_dnsname, std::string *server_addr,
              std::string *server_dnsname);


  // Returns the port the server socket is bound to, or 0 if the socket
  // isn't bound.
  uint16_t port();

 private:
  uint16_t port_;
  int sock_family_;  // either AF_INET or AF_INET6 for ipv4 or ipv6
};

}  // namespace hw5_net

#endif  // _HW5_SERVERSOCKET_H_
