cmake_minimum_required(VERSION 3.5)
project(dispatch)

include(cmake/dispatch_add_benchmark.cmake)
include(cmake/dispatch_add_executable.cmake)

include_directories(include)

# Examples
dispatch_add_executable(switch_table src/switch_table.cpp)
dispatch_add_executable(string_hash src/string_hash.cpp)
dispatch_add_executable(callback_table src/callback_table.cpp)
dispatch_add_executable(map src/map.cpp)

dispatch_add_executable(callback_table_test test/callback_table.cpp)
# dispatch_add_executable(map test/map.cpp)
dispatch_add_executable(linear_hash_test test/linear_hash.cpp)
dispatch_add_executable(sequential_table_test test/sequential_table.cpp)
dispatch_add_executable(chd_test test/chd.cpp)
dispatch_add_executable(switch_table_test test/switch_table.cpp)


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
