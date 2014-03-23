include_directories(BEFORE .)
set(bf_sources
  bf.cc
  configuration.cc
  )

add_executable(bf ${bf_sources})
target_link_libraries(bf libbf)
