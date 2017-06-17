# Copyright Jacqueline Kay 2017
# Distributed under the MIT License.
# See accompanying LICENSE.md or https://opensource.org/licenses/MIT

cmake_minimum_required(VERSION 3.2)
project(petra)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(PETRA_USE_CPP14
  "Build with C++14 compatibility (currently a limited subset of library)." OFF)

if(PETRA_USE_CPP14)
  set(PETRA_CXX_STD "c++14")
  add_definitions(-DPETRA_ENABLE_CPP14)
  # Find Hana
  if(PETRA_HANA_INCLUDE_PATH)
    set(Hana_INCLUDE_DIR ${PETRA_HANA_INCLUDE_PATH})
  else()
    find_package(Hana REQUIRED)
  endif()
else()
  set(PETRA_CXX_STD "c++1z")
endif()

# Compiler check
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT PETRA_USE_CPP14)
    message(FATAL_ERROR
"Petra only builds with Clang 5, and you have selected ${CMAKE_CXX_COMPILER_ID}
without selecting C++14 compatibility.
Please set CMAKE_CXX_COMPILER to the correct executable and rerun CMake, or
configure with -DPETRA_CPP14.")
endif()

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0 AND NOT PETRA_USE_CPP14)
    message(FATAL_ERROR
"Petra only builds with Clang 5, and you have selected an earlier version
without selecting C++14 compatibility.\n
Please build Clang from source download a nightly build from apt.llvm.org, or
configure with -DPETRA_CPP14.")
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
