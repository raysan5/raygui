cmake_minimum_required(VERSION 3.11)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(raygui C)

# Config options
option(BUILD_EXAMPLES "Build the examples." ON)

add_subdirectory(src)

if (${BUILD_EXAMPLES})
  add_subdirectory(examples)
endif()

# TODO: Add automated testing.
# enable_testing()