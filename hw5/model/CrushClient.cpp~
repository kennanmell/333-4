#include <jansson.h>
#include "../shared/Array2D.h"
#include <stdlib.h>
#include "../view/CrushView.h"
#include <iostream>

#include "../shared/ClientSocket.h"
#include "../shared/ServerSocket.h"

using namespace std;

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
    string helloMessage = "hello";
    clientSocket.WrappedWrite(helloMessage.c_str(), helloMessage.length());
  } catch(string errString) {
    cout << errString << endl;
    return 1;
  }
  printf("exiting\n");
  return 0;
}
