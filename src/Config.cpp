#include <GatewayCat/Config.hpp>

void nathcat::gateway::config::to_json(json &j, const config &c) {
  j = json({{"ports", c.ports}, {"threadCount", c.threadCount}});
}

void nathcat::gateway::config::from_json(const json &j, config &c) {
  j.at("ports").get_to(c.ports);
  j.at("threadCount").get_to(c.threadCount);
}

nathcat::gateway::config::config nathcat::gateway::config::get_config() {
  std::fstream file(GATEWAYCAT_CONFIG_FILE_PATH);

  return json::parse(file).get<config>();
}
