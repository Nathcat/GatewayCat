cmake_minimum_required(VERSION 3.31.11)

project(GatewayCat)

add_subdirectory(json-cpp)

add_library(GatewayCatLib STATIC
  src/Config.cpp
  src/Acceptor.cpp
  src/Proxy.cpp
  src/Command.cpp
)

add_executable(GatewayCat
  src/main.cpp
)

target_include_directories(GatewayCatLib PUBLIC
  include
  json-cpp/include
)

target_link_libraries(GatewayCatLib PUBLIC
  nlohmann_json::nlohmann_json
)

target_include_directories(GatewayCat PUBLIC include)
target_link_libraries(GatewayCat PUBLIC GatewayCatLib)
