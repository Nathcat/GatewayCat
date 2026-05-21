#ifndef GATEWAYCAT_LAYERS_PROXY
#define GATEWAYCAT_LAYERS_PROXY

#include "../GatewayCat.hpp"
#include "../Pipeline.hpp"
#include <cstdint>
#include <unistd.h>
using namespace nathcat::gateway;

#include <unordered_map>

namespace nathcat {
namespace gateway {
namespace pipeline {
template <> nathcat::gateway::pipeline::Layer<InSocket>::Layer();
template <>
nathcat::gateway::pipeline::Layer<InSocket>::Layer(Layer<InSocket> &l);
} // namespace pipeline
namespace layers {

class LocalProxy : public pipeline::Layer<InSocket> {
private:
  std::unordered_map<uint16_t, uint16_t> portMap;
  Protocol protocol;

public:
  LocalProxy(std::unordered_map<uint16_t, uint16_t> portMap, Protocol protocol)
      : pipeline::Layer<InSocket>() {
    this->portMap = portMap;
    this->protocol = protocol;
  }

  LocalProxy(LocalProxy &p) : pipeline::Layer<InSocket>(p) {
    this->portMap = p.portMap;
    this->protocol = p.protocol;
  }

  /**
   * @brief While the pipeline is running, wait for a new socket to become
   * available, when one is, begin proxying it.
   */
  void operator()() override;
};
} // namespace layers
} // namespace gateway
} // namespace nathcat

#endif
