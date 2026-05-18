cmake_minimum_required(VERSION 3.31.11)

project(GatewayCat)

add_subdirectory(json-cpp)

add_executable(GatewayCat
  src/main.cpp

  src/Config.cpp
)

target_include_directories(GatewayCat PUBLIC
  include
  json-cpp/include
)

target_link_libraries(GatewayCat PUBLIC
  nlohmann_json::nlohmann_json
)
