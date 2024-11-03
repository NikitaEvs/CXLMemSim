cmake_minimum_required(VERSION 3.11.0)
project(CXLMemSim VERSION 0.1.0)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 23)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif ()

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(
  cxxopts
  URL https://github.com/jarro2783/cxxopts/archive/refs/tags/v3.2.0.zip
)
FetchContent_MakeAvailable(cxxopts)

FetchContent_Declare(
  fmt
  URL https://github.com/fmtlib/fmt/releases/download/11.0.2/fmt-11.0.2.zip
)
FetchContent_MakeAvailable(fmt)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

execute_process(COMMAND uname -r OUTPUT_VARIABLE arch OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "-Wall -fPIC -pthread -ldl -lrt -mavx512f -mpreferred-stack-boundary=4 -g -O0")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(microbench)

add_executable(CXLMemSim ${SOURCE_FILES} src/main.cc)

include_directories(CXLMemSim include ${cxxopts_INCLUDE_DIR} ${fmt_INCLUDE_DIR})
target_link_libraries(CXLMemSim fmt::fmt cxxopts::cxxopts)

add_library(CXLMemSimHook SHARED src/module.cc)
add_executable(CXLMemSimSock ${SOURCE_FILES} src/sock.cc)
target_link_libraries(CXLMemSimSock fmt::fmt cxxopts::cxxopts)
