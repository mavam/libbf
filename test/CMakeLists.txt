add_subdirectory(bf)

if (Boost_FOUND)
  add_executable(unit unit.cc)
  target_link_libraries(unit libbf ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
  add_custom_target(test ${CMAKE_CURRENT_BINARY_DIR}/unit DEPENDS unit libbf)
endif ()
