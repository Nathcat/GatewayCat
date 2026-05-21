#include <GatewayCat/Config.hpp>
#include <GatewayCat/GatewayCat.hpp>
#include <GatewayCat/Layers/Proxy.hpp>
#include <GatewayCat/Pipeline.hpp>
#include <iostream>
#include <vector>
using namespace nathcat::gateway;

int main() {
  bool running = true;
  pipeline::Pipeline<InSocket> pipeline(
      &running, {new layers::LocalProxy({{htons(1234), htons(80)}}, TCP)});

  Acceptor acceptor(&running, htons(1234), 10, TCP);

  std::thread acceptorThread = start_acceptor(acceptor);
  pipeline.start(1);

  char input[1024];

  while (std::strcmp(input, "q")) {
    std::cout << "GatewayCat CLI $ ";
    std::cin >> input;
  }

  std::cout << "Stopping threads..." << std::endl;
  running = false;
  std::cout << "Waiting for threads to stop (this may take a while)..."
            << std::endl;
  acceptorThread.join();
  pipeline.waitForStop();

  std::cout << "All threads have joined. Goodbye! :3" << std::endl;
}
