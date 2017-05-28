cmake_minimum_required(VERSION 3.5)
project(dispatch)

include(cmake/dispatch_add_benchmark.cmake)
include(cmake/dispatch_add_executable.cmake)

include_directories(include)

dispatch_add_executable(generic_jump_table src/generic_jump_table.cpp)

# TODO: will probably take this out unless performance is drastically better
# Find `vrm_pp`:
if(BUILD_BENCHMARKS)
    vrm_cmake_find_extlib(vrm_pp)

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/extlibs/vrm_cmake/cmake/")
    include(vrm_cmake)

    add_executable(macro_switch_table src/macro_switch_table.cpp)
    target_include_directories(macro_switch_table PUBLIC ${VRM_PP_INC_DIR})
endif()

dispatch_add_executable(switch_table src/switch_table.cpp)

dispatch_add_executable(string_hash src/string_hash.cpp)
dispatch_add_executable(simple_string_hash src/simple_string_hash.cpp)

dispatch_add_executable(simple_string_hash_test test/simple_string_hash.cpp)
dispatch_add_executable(exhaustive_string_hash_test test/exhaustive_string_hash.cpp)
dispatch_add_executable(naive_string_hash_test test/naive_string_hash.cpp)
dispatch_add_executable(switch_table_test test/switch_table.cpp)
dispatch_add_executable(unsequenced_jump_table_test test/unsequenced_jump_table.cpp)

# Generate benchmarks

if(BUILD_BENCHMARKS)
    add_custom_target(benchmarks)

    foreach(N RANGE 5 50 5)
      dispatch_add_benchmark(simple_string_hash
          ${N} 7 1000000
          ${CMAKE_BINARY_DIR}/generated/simple_benchmark)
      dispatch_add_benchmark(naive_string_hash
          ${N} 7 1000000
          ${CMAKE_BINARY_DIR}/generated/naive_benchmark -DNAIVE)
endforeach()

endif()
