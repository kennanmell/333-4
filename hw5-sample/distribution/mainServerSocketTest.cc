#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

#include "ServerSocket.h"
#include "ClientSocket.h"

void usage(const char *exeName) {
  cout << "Usage: " << exeName << " [port]" << endl;
  cout << "  Creates a server socket on port, if given," << endl
       << "  or on random port, if not." << endl;
  exit(1);
}

int main(int argc, char *argv[]) {

  if ( argc != 1 && argc != 2 ) usage(argv[0]);
  
  int port = 0;
  try {
    if ( argc == 2 ) port = stoi(argv[1]);
  } catch (...) {
    usage(argv[0]);
  }

  try {
    int socketFd;
    hw5_net::ServerSocket sock(port);
    sock.BindAndListen(AF_INET, &socketFd);
    cout << "Created bound socket. port = " << sock.port() << endl;

    // wait for client connection
    int acceptedFd;
    string clientAddr;
    uint16_t clientPort;
    string clientDNSName;
    string serverAddress;
    string serverDNSName;

    sock.Accept( &acceptedFd,
		 &clientAddr,
		 &clientPort,
		 &clientDNSName,
		 &serverAddress,
		 &serverDNSName );

    cout << "Got connection from: " << endl
	 << "\tacceptedFd\t" << acceptedFd << endl
	 << "\tclientPort\t" << clientPort << endl
	 << "\tclientDNSName\t" << clientDNSName << endl
	 << "\tserverAddress\t" << serverAddress << endl
	 << "\tserverDNSName\t" << serverDNSName << endl;

    // wrap connection to peer with a CientSocket
    hw5_net::ClientSocket peerSocket(acceptedFd);

    // Read and print input until EOF.

    cout << "Reading" << endl;

    char buf[1024];
    int readCount;
    while ( (readCount = peerSocket.WrappedRead(buf, 1023)) ) {
      // write to stdout
      buf[readCount] = '\0'; // make sure buf holds a c style string
      cout << "Got '" << buf << "'" << endl;
      // write back to peer
      peerSocket.WrappedWrite(buf, readCount);
    }
  } catch (string errString) {
    cerr << errString << endl;
    return EXIT_FAILURE;
  }
  
  cout << "Closing" << endl;
  
  return EXIT_SUCCESS;
}
