target_sources(
  ${CUTE_LIBRARY}
    PRIVATE
      cute.cpp
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/cute.impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/cute.h>)
