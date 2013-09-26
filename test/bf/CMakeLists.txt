include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
set(bf_sources
  util/configuration.cc
  bf.cc
  configuration.cc
  hash_table_bloom_filter.cc
  )

add_executable(bf ${bf_sources})
target_link_libraries(bf libbf)
