cmake_minimum_required(VERSION 2.8)

project(libBf)

add_definitions(-Wall -Wno-unused)
if (CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-std=c++0x)
endif ()

set(CMAKE_BUILD_TYPE Debug)

find_package(Boost 1.46
    COMPONENTS 
        program_options
        thread
        unit_test_framework 
        REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(src)
add_subdirectory(test)
