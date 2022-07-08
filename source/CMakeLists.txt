add_subdirectory(cute)

if(CUTE_ENABLE_TESTING)
  add_subdirectory(test)
endif()

if(CUTE_ENABLE_EXAMPLE)
  add_subdirectory(example)
endif()
