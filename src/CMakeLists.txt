set(libbf_sources
  bitvector.cc
  counter_vector.cc
  hash.cc
  bloom_filter/a2.cc
  bloom_filter/basic.cc
  bloom_filter/counting.cc
  bloom_filter/stable.cc
  )

add_library(libbf SHARED ${libbf_sources})
set_target_properties(libbf PROPERTIES OUTPUT_NAME bf)
install(TARGETS libbf DESTINATION lib)
