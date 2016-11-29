#include <jansson.h>
#include "../shared/Array2D.h"
#include <stdlib.h>
#include "../view/CrushView.h"
#include <iostream>

#include "../shared/ClientSocket.h"
#include "../shared/ServerSocket.h"

using namespace std;

int BUFSIZE = 1024;

int main(int argc, char** argv){
  if (argc != 3){
    printf("Usage: ./CrushMain.cpp host port");
    return 1;
  }
  int serverPort;
  try {
    serverPort = stoi(argv[2]);
  } catch (...){
  }

  try {
    string serverName(argv[1]);
    hw5_net::ClientSocket clientSocket(serverName, serverPort);
    string helloMessage = "{\"action\": \"hello\"}";
    clientSocket.WrappedWrite(helloMessage.c_str(), helloMessage.length());
    int readCount;
    cout << "\tRead:" << endl;
    do {
      printf("STARTING A READ\n");
      char buf[BUFSIZE];
      readCount = clientSocket.WrappedRead(buf, BUFSIZE - 1);
      buf[readCount] = '\0';
      cout << buf;
      cout.flush();
      printf("%d\n", readCount);
    } while (readCount != 0);
    cout.flush();
    cout << "done" << endl;
  } catch(string errString) {
    cout << errString << endl;
    return 1;
  }
  printf("exiting\n");
  return 0;
}
