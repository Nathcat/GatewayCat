#ifndef GATEWAYCAT_CLI
#define GATEWAYCAT_CLI

#include <regex>
namespace nathcat {
namespace gateway {
namespace cli {
class Command {
private:
  std::string regexSrc;
  std::regex regex;
  std::string description;

public:
  Command(std::string regex, std::string description) {
    this->regexSrc = regex;
    this->regex = std::regex(regex);
    this->description = description;
  }

  /**
   * @brief Check if a given string buffer matches the expected format of this
   * command
   *
   * @param buffer The string buffer to check
   * @return true if the buffer matches the expected command format
   */
  bool match(char *buffer);

  virtual std::string help();

  /**
   * @brief Run the command on a set of arguments
   *
   * @param args The arguments
   * @param N The size of the arguments list
   */
  virtual void operator()(void *args, int N) = 0;
};
} // namespace cli
} // namespace gateway
} // namespace nathcat

#endif
