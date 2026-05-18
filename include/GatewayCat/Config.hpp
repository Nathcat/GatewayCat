#ifndef GATEWAYCAT_CONFIG
#define GATEWAYCAT_CONFIG

#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
using json = nlohmann::json;

#define GATEWAYCAT_CONFIG_FILE_PATH "Assets/config.json"

namespace nathcat {
namespace gateway {
namespace config {

/**
 * @class config
 * @brief Config object describing configuration values for the server.
 *
 */
struct config {
  /**
   * @brief A list of ports which the server is to manage
   */
  std::vector<int> ports;
  /**
   * @brief The number of threads to run
   */
  int threadCount;
};

/**
 * @brief Facilitates conversion from config object to json
 */
void to_json(json &j, const config &c);

/**
 * @brief Facilitates conversion from json to config object
 */
void from_json(const json &j, config &c);

/**
 * @brief Get the JSON config for the program
 */
config get_config();
} // namespace config
} // namespace gateway
} // namespace nathcat
#endif
