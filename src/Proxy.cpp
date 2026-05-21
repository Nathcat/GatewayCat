#include <GatewayCat/Layers/Proxy.hpp>
#include <iostream>

template <> nathcat::gateway::pipeline::Layer<InSocket>::Layer() {}

void nathcat::gateway::layers::LocalProxy::operator()() {
  while (getParent()->running()) {
    // Get the next socket
    InSocket next;

    if (!getNext(&next))
      break;

    // Open a socket to the proxy target
    in_port_t targetPort = portMap.at(next.addr.sin_port);
    InSocket target;
    target.addr.sin_port = targetPort;
    target.addr.sin_addr.s_addr = GATEWAYCAT_IN_ADDR;
    target.addr.sin_family = GATEWAYCAT_LL_PROTOCOL;

    target.sock = socket(GATEWAYCAT_LL_PROTOCOL, protocol, 0);
    int r = connect(target.sock, (struct sockaddr *)&target.addr,
                    sizeof(target.addr));

    // On error, the proxied socket is closed
    if (r != 0) {
      std::cerr << "Layer #" << getId() << " -> Socket connection failed!"
                << std::endl;
      close(next.sock);
    }

    // Stream data between the proxied sockets until an EOF from the input
    // socket
    char buffer[1];
    while ((r = recv(next.sock, buffer, sizeof(buffer), 0)) != 0) {
      send(target.sock, buffer, sizeof(buffer), 0);
    }

    close(target.sock);
  }
}
