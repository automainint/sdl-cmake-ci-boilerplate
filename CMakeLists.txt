cmake_minimum_required(VERSION 3.16)

set(EXAMPLE example)

if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_BUILD_PARALLEL_LEVEL 3)
endif()

project(
  sdl_cmake_ci_builerplate
  VERSION 0.1.1
  DESCRIPTION "SDL CMake CI boilerplate"
  LANGUAGES C)

include(FetchContent)
enable_testing()

find_package(SDL2 QUIET)

if(NOT SDL2_FOUND)
  FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main)

  FetchContent_MakeAvailable(SDL2)

  add_library(SDL2::SDL2 ALIAS SDL2)
  add_library(SDL2::SDL2main ALIAS SDL2main)
endif()

add_executable(${EXAMPLE})

target_link_libraries(
  ${EXAMPLE}
  PRIVATE
  SDL2::SDL2 SDL2::SDL2main)

target_compile_features(${EXAMPLE} PUBLIC c_std_11)

add_subdirectory(source)
