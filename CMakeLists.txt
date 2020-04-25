cmake_minimum_required(VERSION 3.11)

project(raygui C)

find_package(raylib QUIET)

if(NOT raylib_FOUND)
  message(WARNING "RayLib not found. Building from source.")
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindRaylib.cmake)
endif()

# Config options
option(BUILD_EXAMPLES "Build the examples." ON)

add_subdirectory(src)

if (${BUILD_EXAMPLES})
  add_subdirectory(examples)
endif()

# TODO: Add automated testing.
# enable_testing()
