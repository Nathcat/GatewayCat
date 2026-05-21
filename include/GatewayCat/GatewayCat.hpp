#ifndef GATEWAYCAT
#define GATEWAYCAT

#include "Pipeline.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
using namespace nathcat::gateway;

#define GATEWAYCAT_LL_PROTOCOL AF_INET
#define GATEWAYCAT_IN_ADDR INADDR_ANY

namespace nathcat {
namespace gateway {

enum Protocols { TCP = SOCK_STREAM, UDP = SOCK_DGRAM };

typedef int Protocol;

struct InSocket {
  int sock;
  sockaddr_in addr;
};

/**
 * @class Acceptor
 * @brief To be executed as a thread. Accepts connections from the provided port
 * over the provided protocol, and passes the accepted sockets on to a handler.
 *
 */
class Acceptor {
private:
  InSocket sock;
  sockaddr_in sockAddr;
  bool *runningState;
  int queueSize;
  pipeline::Pipeline<InSocket> *pipeline;

public:
  /**
   * @brief Setup the acceptor. Initialises the internal socket and binds to the
   * specified port / protocol.
   *
   * @param runningState Pointer to value which will control the execution of
   * this thread. This thread will run if the value at this pointer is true.
   * @param port The port this acceptor will monitor
   * @param protocol The protocol this acceptor will accept
   */
  Acceptor(bool *runningState, int port, int queueSize, Protocol protocol);
  ~Acceptor();

  Acceptor(Acceptor &o);

  Acceptor &operator=(Acceptor &o);

  /**
   * @brief Execution flow for this acceptor
   */
  void operator()();
};

/**
 * @brief Starts an acceptor and returns the thread in which it is running
 *
 * @param acceptor The acceptor to run
 */
std::thread start_acceptor(Acceptor &acceptor);

}; // namespace gateway
}; // namespace nathcat
#endif
