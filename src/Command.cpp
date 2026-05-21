#include <GatewayCat/CLI.hpp>
using namespace nathcat::gateway;

bool nathcat::gateway::cli::Command::match(char *buffer) {
  return std::regex_match(buffer, regex);
}

std::string nathcat::gateway::cli::Command::help() {
  std::string s = "";
  s = s.append(regexSrc).append(" - ").append(description);

  return s;
}
