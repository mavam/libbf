add_subdirectory(bf)

enable_testing()
add_executable(bf-test tests.cpp)
target_link_libraries(bf-test libbf ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME unit COMMAND bf-test)
