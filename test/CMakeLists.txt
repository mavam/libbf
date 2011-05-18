file(GLOB_RECURSE deps bf.cc configuration.cc metrics.cc)
add_executable(bf ${deps})
target_link_libraries(bf ${Boost_LIBRARIES})

add_executable(unit unit.cc)
target_link_libraries(unit ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
