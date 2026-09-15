cmake_minimum_required(VERSION 3.14)
project(HyperLog VERSION 1.0.1 LANGUAGES CXX)

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

add_executable(
	HyperLog 
	src/main.cc 
	
	# core
	src/core/log_entry.h
	src/core/log_parser.h
	src/core/log_parser.cc
	src/core/log_stream.h
	src/core/log_stream.cc
	src/core/generator.h

	# ui
	src/ui/app.h 
	src/ui/app.cc
)

 file(COPY test.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(HyperLog
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE simdjson
)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
