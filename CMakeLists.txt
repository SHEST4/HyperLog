cmake_minimum_required(VERSION 3.14)
project(HyperLog VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG main 
)
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(
  simdjson
  GIT_REPOSITORY https://github.com/simdjson/simdjson
  GIT_TAG master 
)
FetchContent_MakeAvailable(simdjson)

add_executable(HyperLog src/main.cc)

target_link_libraries(HyperLog
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE simdjson
)