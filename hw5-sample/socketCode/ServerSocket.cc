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

#include <string>
#include <cstdio>       // for snprintf()
#include <cstring>
#include <unistd.h>      // for close(), fcntl()
#include <sys/types.h>   // for socket(), getaddrinfo(), etc.
#include <sys/socket.h>  // for socket(), getaddrinfo(), etc.
#include <arpa/inet.h>   // for inet_ntop()
#include <netdb.h>       // for getaddrinfo()
#include <errno.h>       // for errno, used by strerror()
#include <string.h>      // for memset, strerror()
#include <iostream>      // for std::cerr, etc.

using std::string;

#include "./ServerSocket.h"

extern "C" {
  #include "CSE333.h"
}

namespace hw5_net {

  ServerSocket::ServerSocket(uint16_t port) {
    if ( port > 0 ) port_ = port;
    else {
      port_ = 10000;
      port_ += ((uint16_t) getpid()) % 25000;
      port_ += ((uint16_t) rand()) % 5000;  // NOLINT(runtime/threadsafe_fn)  
    }
    socketFD_ = -1;
  }

  ServerSocket::~ServerSocket() {
    if ( socketFD_ >= 0 ) close(socketFD_);
    socketFD_ = -1;
  }

  uint16_t ServerSocket::port() {
    if ( socketFD_ == -1 ) return 0;
    return port_;
  }

  void ServerSocket::BindAndListen(int ai_family, int *listen_fd) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int res;

    // Defensive programming.
    Verify333((ai_family == AF_INET) || (ai_family == AF_INET6) ||
	      (ai_family == AF_UNSPEC));

    // Populate the "hints" addrinfo structure for getaddrinfo().
    // ("man getaddrinfo" -- it's a complicated system call but a
    // very useful one.)
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = ai_family;
    hints.ai_socktype = SOCK_STREAM;  // stream
    hints.ai_flags = AI_PASSIVE;      // use wildcard "INADDR_ANY"
    hints.ai_protocol = IPPROTO_TCP;  // tcp protocol
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    // Create a string representation of our port number to pass
    // in to getaddrinfo().
    char portnum[10];  // 10 bytes is plenty of space for a uint16_t.
    snprintf(portnum, sizeof(portnum), "%u", (unsigned int) port_);

    // Run getaddrinfo(); it returns a list of address structures
    // via the output parameter into our "result" variable.
    res = getaddrinfo(NULL, portnum, &hints, &result);
    if (res != 0) {
      throw string("ServerSocket::BindAndListen: ") + gai_strerror(res);
    }

    // Loop through the returned address structures until we are able to
    // successfully create a socket() and bind() to one of them.
    for (rp = result; rp != NULL; rp = rp->ai_next) {
      socketFD_ = socket(rp->ai_family, rp->ai_socktype,
			 rp->ai_protocol);
      if (socketFD_ == -1) {
	continue;
      }

      // Set a socket option to enable re-use of the port number as
      // soon as the server exits. ("man setsockopt")
      int optval = 1;
      Verify333(setsockopt(socketFD_,
			   SOL_SOCKET,
			   SO_REUSEADDR,
			   &optval,
			   sizeof(optval)) == 0);

      if (bind(socketFD_, rp->ai_addr, rp->ai_addrlen) == 0) {
	// success!
	sock_family_ = rp->ai_family;
	Verify333((sock_family_ == AF_INET) || (sock_family_ == AF_INET6));
	break;
      }

      // This address didn't work so close the socket, loop back, and
      // try again with the next one returned by getaddrinfo().
      close(socketFD_);
      socketFD_ = -1;
    }

    // We've finished looking through the getaddrinfo() results.
    if (rp == NULL) {
      // We didn't successfully socket() and bind() any of the
      // addresses, so we need to return failure.
      freeaddrinfo(result);
      throw string("ServerSocket::BindAndListen: Couldn't find any addresses to bind to.");
    }

    // Mark the socket as a listening socket.
    if (listen(socketFD_, SOMAXCONN) != 0) {
      freeaddrinfo(result);
      close(socketFD_);
      socketFD_ = -1;
      throw string("ServerSocket:BindAndListen: Failed to mark socket as listening: ") + strerror(errno);
    }

    // Return the socket through the output parameter, then clean up and
    // return success.
    *listen_fd = socketFD_;
    freeaddrinfo(result);
  }

  //----------------------------------------------------------
  // Acccept()
  //----------------------------------------------------------

  bool ServerSocket::Accept(int *accepted_fd,
			    std::string *client_addr,
			    uint16_t *client_port,
			    std::string *client_dnsname,
			    std::string *server_addr,
			    std::string *server_dnsname) {
    int peer_fd;
    struct sockaddr_in peer4;
    struct sockaddr_in6 peer6;
    struct sockaddr *peer;
    socklen_t peerlen;

    // Do I need an IPv4 or IPv6 sockaddr_in/sockaddr_in6 structure?
    if (sock_family_ == AF_INET) {
      // IPv4
      peer = (struct sockaddr *) &peer4;
      peerlen = sizeof(peer4);
    } else {
      // IPv6
      peer = (struct sockaddr *) &peer6;
      peerlen = sizeof(peer6);
    }

    // Try to accept a connection; if we get EINTR, loop and try again.
    while (1) {
      peer_fd = accept(socketFD_, peer, &peerlen);
      if (peer_fd == -1) {
	if (errno == EINTR) {
	  //continue;
	  *accepted_fd = -1;
	  return false;
	}
	// We experienced some kind of failure; print the message to
	// the console and return failure to the caller.
	// std::cerr << "accept failed: " << strerror(errno) << std::endl;
	throw string("ServerSocket::Accept: ") + std::strerror(errno);
      }
      // We successfully accept()'ed a connection.
      break;
    }

    // We now have a connection to a client.  Set the socket options to
    // enable re-use of the port, similar to our listening socket.
    int optval = 1;
    Verify333(setsockopt(peer_fd, SOL_SOCKET, SO_REUSEADDR,
			 &optval, sizeof(optval)) == 0);

    // Gather some information about the connection to return to our
    // customer.  Start with information about the client.
    char hname[1024];  // Ought to be enough for most DNS names.
    hname[0] = '\0';
    if (sock_family_ == AF_INET) {
      // Get the client's IPv4 address in string form.
      char addrbuf[INET_ADDRSTRLEN];
      Verify333(inet_ntop(AF_INET, &peer4.sin_addr,
			  addrbuf, INET_ADDRSTRLEN) != NULL);
      *client_addr = std::string(addrbuf);
      // Get the client's port number, in host order.
      *client_port = ntohs(peer4.sin_port);
      // Try to get the client's dns name; if that fails, return the
      // client's IP address as a substitute.
      if (getnameinfo((const struct sockaddr *) &peer4,
		      peerlen, hname, 1024, NULL, 0, 0) != 0) {
	*client_dnsname = *client_addr;
      } else {
	*client_dnsname = std::string(hname);
      }
    } else {
      // Get the client's IPv6 address.
      char addrbuf[INET6_ADDRSTRLEN];
      Verify333(inet_ntop(AF_INET6, &peer6.sin6_addr,
			  addrbuf, INET6_ADDRSTRLEN) != NULL);
      *client_addr = std::string(addrbuf);
      // Get the client's port number, in host order.
      *client_port = ntohs(peer6.sin6_port);
      // Get the client's dns name; if that fails, return the client's
      // IP address as a substitute.
      if (getnameinfo((const struct sockaddr *) &peer6,
		      peerlen, hname, 1024, NULL, 0, 0) != 0) {
	*client_dnsname = *client_addr;
      } else {
	*client_dnsname = std::string(hname);
      }
    }

    // Now gather information about the server-side of the connection.
    if (sock_family_ == AF_INET) {
      // The server is using an IPv4 address.
      struct sockaddr_in srvr;
      socklen_t srvrlen = sizeof(srvr);
      char addrbuf[INET_ADDRSTRLEN];
      Verify333(getsockname(peer_fd,
			    (struct sockaddr *) &srvr,
			    &srvrlen) == 0);
      Verify333(inet_ntop(AF_INET, &srvr.sin_addr,
			  addrbuf, INET_ADDRSTRLEN) != NULL);
      *server_addr = std::string(addrbuf);
      // Get the server's dns name, or return it's IP address as
      // a substitute if the dns lookup fails.
      if (getnameinfo((const struct sockaddr *) &srvr,
		      srvrlen, hname, 1024, NULL, 0, 0) != 0) {
	*server_dnsname = *server_addr;
      } else {
	*server_dnsname = std::string(hname);
      }
    } else {
      // The server is using an IPv6 address.
      struct sockaddr_in6 srvr;
      socklen_t srvrlen = sizeof(srvr);
      char addrbuf[INET6_ADDRSTRLEN];
      Verify333(getsockname(peer_fd,
			    (struct sockaddr *) &srvr,
			    &srvrlen) == 0);
      Verify333(inet_ntop(AF_INET6, &srvr.sin6_addr,
			  addrbuf, INET6_ADDRSTRLEN) != NULL);
      *server_addr = std::string(addrbuf);
      // Get the server's dns name, or return it's IP address as
      // a substitute if the dns lookup fails.
      if (getnameinfo((const struct sockaddr *) &srvr,
		      srvrlen, hname, 1024, NULL, 0, 0) != 0) {
	*server_dnsname = *server_addr;
      } else {
	*server_dnsname = std::string(hname);
      }
    }

    // Return the socket to the caller
    *accepted_fd = peer_fd;
    return true;
  }

}  // namespace hw5_net
