include_directories(BEFORE .)
set(bf_sources
  util/configuration.cc
  bf.cc
  configuration.cc
  metrics.cc
  )

add_executable(bf ${bf_sources})
target_link_libraries(bf libbf)
