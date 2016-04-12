add_subdirectory(bf)

add_executable(bf-test tests.cpp)
target_link_libraries(bf-test libbf)
add_custom_target(test 
  ${CMAKE_CURRENT_BINARY_DIR}/bf-test DEPENDS bf-test libbf)
