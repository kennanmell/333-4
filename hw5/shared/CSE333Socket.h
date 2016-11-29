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

#ifndef _CSE333SOCKET_H_
#define _CSE333SOCKET_H_

#include <cstdio>
#include "unistd.h"

namespace hw5_net {

  class CSE333Socket {
  public:
    // Returns the socket's file descriptor (for use with read, write,
    // close, etc.)
    // Returns -1 for errors;
    int getAsFileDescriptor() { return socketFD_; }

  protected:
    // it's a mistake for client code to create one of these.
    // Instead, create a ClientSocket or a ServerSocket.
    CSE333Socket() : socketFD_(-1) {}

    // destructor closes the file descriptor
    virtual ~CSE333Socket();
    
    int socketFD_;  // file descriptor.  -1 for uninitialized

  };


}  // namespace hw5_net

#endif  // _HW5_CLIENTSOCKET_H_
