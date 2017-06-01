cmake_minimum_required(VERSION 3.5)
project(dispatch)

include(cmake/dispatch_add_executable.cmake)
include(cmake/dispatch_add_test.cmake)

# Examples
dispatch_add_executable(switch_table src/switch_table.cpp)
dispatch_add_executable(string_hash src/string_hash.cpp)
dispatch_add_executable(callback_table src/callback_table.cpp)
dispatch_add_executable(map src/map.cpp)

include(CTest)

dispatch_add_test(callback_table test/callback_table.cpp)
dispatch_add_test(map test/map.cpp)
dispatch_add_test(linear_hash test/linear_hash.cpp)
dispatch_add_test(sequential_table test/sequential_table.cpp)
dispatch_add_test(chd test/chd.cpp)
dispatch_add_test(switch_table test/switch_table.cpp)

# include(cmake/dispatch_add_benchmark.cmake)
#if(BUILD_BENCHMARKS)
#    add_custom_target(benchmarks)
#
#    foreach(N RANGE 5 50 5)
#      dispatch_add_benchmark(simple_string_hash
#          ${N} 7 1000000
#          ${CMAKE_BINARY_DIR}/generated/simple_benchmark)
#      dispatch_add_benchmark(naive_string_hash
#          ${N} 7 1000000
#          ${CMAKE_BINARY_DIR}/generated/naive_benchmark -DNAIVE)
#endforeach()
#
#endif()
