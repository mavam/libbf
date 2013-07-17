include_directories(${CMAKE_CURRENT_BINARY_DIR})

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/bf.h
  DESTINATION include)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bf
  DESTINATION include
  FILES_MATCHING PATTERN "*.h")

add_subdirectory(bf)
