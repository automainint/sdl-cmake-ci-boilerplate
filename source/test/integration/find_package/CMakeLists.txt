cmake_minimum_required(VERSION 3.16)
set(NAME cute-integration-find-package)
project(${NAME}-root)

include(ExternalProject)

ExternalProject_Add(
  cute
  GIT_REPOSITORY  https://github.com/automainint/cute.git
  GIT_TAG         dev
  CMAKE_ARGS
  -D CMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/install
  -D CUTE_ENABLE_TESTING=OFF
  -D CUTE_ENABLE_EXAMPLE=OFF)

ExternalProject_Add(
  ${NAME}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source
  CMAKE_ARGS
  -D CMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/install)

ExternalProject_Add_StepDependencies(${NAME} build cute)

include(GNUInstallDirs)

enable_testing()

add_test(
  NAME ${NAME}-run
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/install/${CMAKE_INSTALL_BINDIR}/${NAME}${CMAKE_EXECUTABLE_SUFFIX})

set_tests_properties(
  ${NAME}-run
  PROPERTIES
  TIMEOUT "15")
