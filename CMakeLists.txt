# Copyright Jacqueline Kay 2017
# Distributed under the MIT License.
# See accompanying LICENSE.md or https://opensource.org/licenses/MIT

cmake_minimum_required(VERSION 3.2)
project(petra)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Compiler check

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
    message(FATAL_ERROR
        "Petra only builds with Clang 5, and you have selected an earlier version.\n
        Please build Clang from source or download a nightly build from apt.llvm.org.")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
    message(FATAL_ERROR
        "Petra only builds with GCC 8, and you have selected an earlier version.\n
        Please build GCC from source.")
endif()


add_library(petra INTERFACE)
target_include_directories(petra INTERFACE include)

option(PETRA_USE_UDL
       "Enable GNU string literal operator template extension."
       ON)

option(PETRA_BUILD_TESTS "Build tests." ON)

add_subdirectory(examples)

if(PETRA_BUILD_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()

install(DIRECTORY include DESTINATION include)
