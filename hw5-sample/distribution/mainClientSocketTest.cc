#include <iostream>
#include <string>
#include <vector>

#include <cstdlib>

#include "ClientSocket.h"

using namespace std;

vector<string> messageList = { "one",
			       "two",
			       "three",
			       "four",
			       "five",
			       "six",
			       "seven",
                              };

void usage(const char *exeName) {
  cout << "Usage: " << exeName << " host port" << endl;
  cout << "  Connect to host::port then send data and read replies" << endl;
  exit(1);
}

int main(int argc, char *argv[]) {

  if ( argc != 3 ) usage(argv[0]);

  int serverPort;
  try {
    serverPort = stoi(argv[2]);
  } catch (...) {
    usage(argv[0]);
  }

  try {
    string serverName(argv[1]);
    hw5_net::ClientSocket clientSocket(serverName, serverPort);

    for (auto &s : messageList ) {
      char buf[1024];
      clientSocket.WrappedWrite(s.c_str(), s.length());
      int readCount = clientSocket.WrappedRead(buf, 1023);
      buf[readCount] = '\0';
      cout << "\tRead '" << buf << "'" << endl;
    }
  } catch(string errString) {
    cerr << errString << endl;
    return EXIT_FAILURE;
  }

  cout << "Exiting..." << endl;

  return EXIT_SUCCESS;
}
