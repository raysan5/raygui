include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 6a8b1079c13b7d5e1bddc0ecc86ad7e16fc2556d
)
FetchContent_GetProperties(raylib)
if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
  set(FETCHCONTENT_QUIET NO)
  FetchContent_Populate(raylib)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
  set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # or games
  add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
endif()
