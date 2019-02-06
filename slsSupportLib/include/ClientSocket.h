#pragma once
#include "DataSocket.h"
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

namespace sls {

class ClientSocket : public DataSocket {
  public:
    ClientSocket(const std::string &hostname, uint16_t port_number);
    int sendCommandThenRead(int fnum, void *args, size_t args_size, void *retval, size_t retval_size);

  private:
    void readReply(int &ret, void *retval, size_t retval_size);
    struct sockaddr_in serverAddr {};
};

}; //namespace sls