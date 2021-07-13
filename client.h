#ifndef CLIENT
#define CLIENT
#include "config.h"

class Client {
public:
  Client();
  bool Connect();
  bool Start();
  void Close();

private:
  int sock;
  char bufSend[BUF_SIZE];
  char bufRecv[BUF_SIZE];
  struct sockaddr_in serv_addr;
};

#endif