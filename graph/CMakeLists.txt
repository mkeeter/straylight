cmake_minimum_required(VERSION 2.6)
project(noonlight)
set(CMAKE_BUILD_TYPE DEBUG)

################################################################################
# Flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-comment -g -fPIC -pedantic -std=c++11 -Werror=switch -march=native")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DRELEASE")
set(CMAKE_CXX_FLAGS_DEBUG    "-O0")

################################################################################
# OS-specific flags
if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()

################################################################################

file(GLOB SOURCE_FILES *.cpp)
add_executable(graph-test test/main.cpp ${SOURCE_FILES} ../s7/s7.c)
target_include_directories(graph-test PRIVATE . ../s7)
