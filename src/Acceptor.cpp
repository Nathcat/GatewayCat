#include <GatewayCat/GatewayCat.hpp>
#include <netinet/in.h>
#include <thread>
using namespace nathcat::gateway;

nathcat::gateway::Acceptor::~Acceptor() { this->runningState = nullptr; }

nathcat::gateway::Acceptor::Acceptor(Acceptor &o) {
  this->runningState = o.runningState;
  this->sockAddr = o.sockAddr;
  this->sock = o.sock;
}

Acceptor &nathcat::gateway::Acceptor::operator=(Acceptor &o) {
  this->runningState = o.runningState;
  this->sockAddr = o.sockAddr;
  this->sock = o.sock;

  return *this;
}

nathcat::gateway::Acceptor::Acceptor(bool *runningState, int port,
                                     int queueSize, Protocol protocol) {
  this->runningState = runningState;
  this->queueSize = queueSize;

  this->sock.sock = socket(GATEWAYCAT_LL_PROTOCOL, protocol, 0);
  struct sockaddr_in sockAddr;
  this->sock.addr.sin_family = GATEWAYCAT_LL_PROTOCOL;
  this->sock.addr.sin_port = htons(port);
  this->sock.addr.sin_addr.s_addr = GATEWAYCAT_IN_ADDR;

  int r = bind(this->sock.sock, (struct sockaddr *)&this->sock.addr,
               sizeof(this->sockAddr));

  if (r != 0)
    throw r;
}

void nathcat::gateway::Acceptor::operator()() {
  listen(sock.sock, queueSize);

  while (runningState) {
    sockaddr peer;
    unsigned int peerAddrLen;
    int clientfd = accept(sock.sock, &peer, &peerAddrLen);

    sockaddr_in *peerCast = (sockaddr_in *)&peer;

    InSocket client = {clientfd, *peerCast};
    (*pipeline) << client;
  }
}

std::thread nathcat::gateway::start_acceptor(Acceptor &acceptor) {
  std::thread t([](Acceptor &acceptor) { acceptor(); }, std::ref(acceptor));

  return t;
}
