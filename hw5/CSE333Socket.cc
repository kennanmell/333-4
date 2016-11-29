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

#include <iostream>

using namespace std;

#include "CSE333Socket.h"

// destructor closes the file descriptor
hw5_net::CSE333Socket::~CSE333Socket() {
  if ( socketFD_ >= 0 ) close(socketFD_);
  socketFD_ = -1;
}
