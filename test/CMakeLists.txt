add_subdirectory(bf)

add_executable(unit unit.cc)
target_link_libraries(unit libbf ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
