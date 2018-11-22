find_package(Threads)
if (NOT Threads_FOUND)
  message(FATAL_ERROR "Could not find system threading libraries")
endif ()

add_subdirectory(bf)

enable_testing()
add_executable(bf-test tests.cpp)
target_link_libraries(bf-test libbf ${CMAKE_THREAD_LIBS_INIT})
add_test(unit ${CMAKE_BINARY_DIR}/bin/bf-test)
