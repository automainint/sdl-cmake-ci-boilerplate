cmake_minimum_required(VERSION 3.16)
set(NAME cute-integration-find-package)
project(${NAME})

find_package(cute)

add_executable(${NAME})
target_sources(${NAME} PRIVATE main.cpp)
target_link_libraries(${NAME} PRIVATE cute::cute)
install(TARGETS ${NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
