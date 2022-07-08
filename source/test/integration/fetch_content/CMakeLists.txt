cmake_minimum_required(VERSION 3.16)
set(NAME cute-integration-fetch-content)
project(${NAME} CXX)

include(FetchContent)
FetchContent_Declare(
  cute
  GIT_REPOSITORY https://github.com/automainint/cute.git
  GIT_TAG dev)
set(CUTE_ENABLE_TESTING OFF)
set(CUTE_ENABLE_EXAMPLE OFF)
FetchContent_MakeAvailable(cute)

add_executable(${NAME})
target_sources(${NAME} PRIVATE main.cpp)
target_link_libraries(${NAME} PRIVATE cute::cute)

enable_testing()
add_test(NAME ${NAME}-run COMMAND ${NAME})
set_tests_properties(${NAME}-run PROPERTIES TIMEOUT "15")
