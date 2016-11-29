/*
 * Copyright 2012 Steven Gribble
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

// This file contains a number of HTTP and HTML parsing routines
// that come in useful throughput the assignment.

#include <arpa/inet.h>
#include <cerrno>
#include <climits>
#include <netdb.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include "./ClientSocket.h"


namespace hw5_net {

  ClientSocket::ClientSocket(string hostname, uint16_t portnum) {
    struct addrinfo hints, *results, *r;
    int clientsock, retval;
    char portstr[10];

    // Convert the port number to a C-style string.
    snprintf(portstr, sizeof(portstr), "%hu", portnum);

    // Zero out the hints data structure using memset.
    memset(&hints, 0, sizeof(hints));

    // Indicate we're happy with either AF_INET or AF_INET6 addresses.
    hints.ai_family = AF_UNSPEC;

    // Constrain the answers to SOCK_STREAM addresses.
    hints.ai_socktype = SOCK_STREAM;

    // Do the lookup.
    if ((retval = getaddrinfo(hostname.c_str(),
			      portstr,
			      &hints,
			      &results)) != 0) {
      throw string("ClientSocket: getaddrinfo failed: ") + gai_strerror(retval);
    }

    // Loop through, trying each out until one succeeds.
    for (r = results; r != NULL; r = r->ai_next) {
      // Try manufacturing the socket.
      if ((clientsock = socket(r->ai_family, SOCK_STREAM, 0)) == -1) {
	continue;
      }
      // Try connecting to the peer.
      if (connect(clientsock, r->ai_addr, r->ai_addrlen) == -1) {
	continue;
      }
      socketFD_ = clientsock;
      freeaddrinfo(results);
      return;
    }
    freeaddrinfo(results);
    throw string("ClientSocket: Couldn't connect to ") + hostname + ":" + portstr;
  }

  int ClientSocket::WrappedRead(char *buf, int readlen) {
    int res;
    if (socketFD_ < 0 ) return -1;
    while (1) {
      res = read(socketFD_, buf, readlen);
      if (res == -1) {
	if ((errno == EAGAIN) || (errno == EINTR))
	  continue;
	throw std::string("CSE333Socket::WrappedRead: " ) + std::strerror(errno);
      }
      break;
    }
    return res;
  }

  int ClientSocket::WrappedWrite(const char *buf, int writelen) {
    int res, written_so_far = 0;

    while (written_so_far < writelen) {
      if ( socketFD_ < 0 ) return -1;
      res = write(socketFD_, buf + written_so_far, writelen - written_so_far);
      if (res == -1) {
	if ((errno == EAGAIN) || (errno == EINTR))
	  continue;
	throw std::string("CSE333Socket::WrappedWrite: " ) + std::strerror(errno);
      }
      if (res == 0)
	break;
      written_so_far += res;
    }
    return written_so_far;
  }
  
}  // namespace hw5_net
