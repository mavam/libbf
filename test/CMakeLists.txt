add_subdirectory(bf)

if (Boost_FOUND)
  include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/bf)
  add_executable(unit unit.cc bf/hash_table_bloom_filter.cc)
  target_link_libraries(unit libbf ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
  add_custom_target(test ${CMAKE_CURRENT_BINARY_DIR}/unit DEPENDS unit libbf)
endif ()
