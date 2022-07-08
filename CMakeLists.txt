cmake_minimum_required(VERSION 3.16)

option(CUTE_ENABLE_TESTING "Enable testing" ON)
option(CUTE_ENABLE_EXAMPLE "Enable example" ON)

set(CUTE_PROJECT cute)
set(CUTE_LIBRARY cute)
set(CUTE_TEST cute-test)
set(CUTE_EXAMPLE cute-example)
set(CUTE_UNITTESTS cute-unittests)
set(CUTE_TARGETS cute-targets)

if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_BUILD_PARALLEL_LEVEL 3)
endif()

project(
  ${CUTE_PROJECT}
  VERSION 0.1.1
  DESCRIPTION "Cute"
  LANGUAGES CXX)

add_library(${CUTE_LIBRARY} STATIC)
add_library(${CUTE_PROJECT}::${CUTE_LIBRARY} ALIAS ${CUTE_LIBRARY})
target_include_directories(
  ${CUTE_LIBRARY} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>)
target_compile_features(${CUTE_LIBRARY} PUBLIC cxx_std_20)

include(FetchContent)
enable_testing()

if(CUTE_ENABLE_TESTING)
  find_package(Catch2 QUIET)

  if(NOT Catch2_FOUND)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v2.13.9)

    FetchContent_MakeAvailable(Catch2)
  endif()

  add_executable(${CUTE_TEST})

  target_link_libraries(
    ${CUTE_TEST}
    PRIVATE
    ${CUTE_LIBRARY} Catch2::Catch2)

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      ${CUTE_LIBRARY} PUBLIC
      -fsanitize=undefined,address
      --coverage -O0 -g)
    target_link_options(
      ${CUTE_LIBRARY} PUBLIC
      -fsanitize=undefined,address
      --coverage)
  endif()

  add_test(
    NAME ${CUTE_UNITTESTS}
    COMMAND ${CUTE_TEST})

  set_tests_properties(
    ${CUTE_UNITTESTS}
    PROPERTIES
    TIMEOUT "15")
endif()

if(CUTE_ENABLE_EXAMPLE)
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

  add_executable(${CUTE_EXAMPLE})

  target_link_libraries(
    ${CUTE_EXAMPLE}
    PRIVATE
    ${CUTE_LIBRARY} SDL2::SDL2 SDL2::SDL2main)

  target_compile_features(${CUTE_EXAMPLE} PUBLIC cxx_std_20)
endif()

add_subdirectory(source)

include(GNUInstallDirs)

file(
  GLOB_RECURSE headers_
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source/cute
  ${CMAKE_CURRENT_SOURCE_DIR}/source/cute/*.h)

foreach(path_ ${headers_})
  get_filename_component(dir_ "${path_}" DIRECTORY)

  install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/cute/${path_}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cute/${dir_})
endforeach()

unset(headers_)
unset(path_)
unset(dir_)

install(
  TARGETS ${CUTE_LIBRARY}
  EXPORT ${CUTE_TARGETS}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
  EXPORT ${CUTE_TARGETS}
  FILE ${CUTE_TARGETS}.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

export(
  EXPORT ${CUTE_TARGETS}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${CUTE_TARGETS}.cmake"
  NAMESPACE ${PROJECT_NAME}::)

include(CMakePackageConfigHelpers)

string(TOLOWER ${PROJECT_NAME} project_lower_)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config-version.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion)

install(
  FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

unset(project_lower_)
