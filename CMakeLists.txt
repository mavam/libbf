# -- Project Setup ------------------------------------------------------------

project(libbf CXX)

include(CTest)

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Support 'make uninstall'.
if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
                  @ONLY)
  add_custom_target(uninstall COMMAND
      ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif ()

# Prohibit in-source builds.
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed. Please use "
                      "./configure to choose a build directory and "
                      "initialize the build configuration.")
endif ()

# Silence warning CMP0042
if (APPLE AND NOT DEFINED CMAKE_MACOSX_RPATH)
  set(CMAKE_MACOSX_RPATH true)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin
  CACHE PATH "Single directory for all executables")

set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib
  CACHE PATH "Single directory for all libraries")

# -- Dependencies -------------------------------------------------------------

find_package(Threads)
if (NOT Threads_FOUND)
  message(FATAL_ERROR "Could not find system threading libraries")
endif ()

set(CMAKE_CXX_FLAGS                "-Wall -Wextra -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

if (ENABLE_DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
else ()
  set(CMAKE_BUILD_TYPE Release)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
    message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
  endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else ()
  message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()

# -- libbf -------------------------------------------------------------------

include_directories(${CMAKE_SOURCE_DIR})

set(libbf_sources
  src/bitvector.cpp
  src/counter_vector.cpp
  src/hash.cpp
  src/bloom_filter/a2.cpp
  src/bloom_filter/basic.cpp
  src/bloom_filter/bitwise.cpp
  src/bloom_filter/counting.cpp
  src/bloom_filter/stable.cpp
)

add_library(libbf_static STATIC ${libbf_sources})
set_target_properties(libbf_static PROPERTIES OUTPUT_NAME "bf")
set_target_properties(libbf_static PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

add_library(libbf_shared SHARED ${libbf_sources})
set_target_properties(libbf_shared PROPERTIES OUTPUT_NAME "bf")
set_target_properties(libbf_shared PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

install(TARGETS libbf_static DESTINATION lib)
install(TARGETS libbf_shared DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bf DESTINATION include)

add_subdirectory(test)
