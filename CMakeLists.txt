cmake_minimum_required(VERSION 2.6)
project(noonlight)
set(CMAKE_BUILD_TYPE RELEASE)

################################################################################
# Flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-comment -g -fPIC -pedantic -std=c++11 -Werror=switch -march=native")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DRELEASE")
set(CMAKE_CXX_FLAGS_DEBUG    "-O0")

################################################################################
# Graphics libraries

# libepoxy
find_path(EPOXY_INCLUDE_DIR epoxy/gl.h)
find_library(EPOXY_LIBRARY NAMES epoxy)

# glfw3
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
add_definitions(-DGLFW_INCLUDE_GLCOREARB)

# OpenGL
find_library(OPENGL_LIBRARIES OpenGL)

# Eigen 3
pkg_search_module(EIGEN REQUIRED eigen3)

################################################################################
# OS-specific flags
if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)

    # Mask error message that warns about including gl.h and gl3.h
    # (since it's actually a false positive caused by epoxy/gl.h)
    add_definitions("-DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED")
endif()

################################################################################

file(GLOB IMGUI_FILES imgui/*.cpp)
add_executable(noonlight main.cpp datum.cpp colors.cpp ui.cpp s7/s7.c ${IMGUI_FILES})

target_include_directories(noonlight SYSTEM PRIVATE
    ${GLFW_INCLUDE_DIRS}
    ${EPOXY_INCLUDE_DIR})
target_include_directories(noonlight PUBLIC s7 imgui)

target_link_libraries(noonlight
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARY}
    ${EPOXY_LIBRARY})
