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

// This file contains a number of HTTP and HTML parsing routines
// that come in useful throughput the assignment.

#ifndef _HW5_CLIENTSOCKET_H_
#define _HW5_CLIENTSOCKET_H_

#include <string>
using std::string;

#include "CSE333Socket.h"

namespace hw5_net {

  class ClientSocket : public CSE333Socket {
  public:
    // constructor creates socket and tries to connect it. Hostname can
    // be a DNS name or an IP address, in string form.
    explicit ClientSocket(string hostname, uint16_t portnum);

    // wraps an existing file descriptor as a ClientSocket
    explicit ClientSocket(int fd) { socketFD_ = fd; }

    // returns true if the socket is connected, false otherwise
    bool isConnected() { return socketFD_ >= 0; }

    // read() can be hard to use.  This method is a wrapper around
    // read that shields the caller from dealing
    // with the ugly issues of partial reads, EINTR, EAGAIN, and so
    // on.
    //
    // Reads at most "readlen" bytes from the file descriptor fd
    // into the buffer "buf".  Returns the number of bytes actually
    // read.  If EOF is hit and no bytes have been read, returns 0.
    // Throws a string exception on fatal error.
    // Might read fewer bytes than requested.
    int WrappedRead(char *buf, int readlen);

    // A wrapper around write that shields the caller from dealing
    // with the ugly issues of partial writes, EINTR, EAGAIN, and so
    // on.
    //
    // Writes "writelen" bytes to the file descriptor fd from
    // the buffer "buf".  Blocks the caller until either writelen
    // bytes have been written, or an error is encountered.  Returns
    // the total number of bytes written. Throws a string exception
    // on a fatal error.
    int WrappedWrite(const char *buf, int writelen);
  };
}  // namespace hw5_net

#endif  // _HW5_CLIENTSOCKET_H_
