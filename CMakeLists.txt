cmake_minimum_required(VERSION 3.5)
project(dispatch)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(dispatch INTERFACE)
target_include_directories(dispatch INTERFACE include)

option(DISPATCH_USE_UDL
       "Enable GNU string literal operator template extension."
       ON)

option(DISPATCH_BUILD_TESTS "Build tests." ON)

add_subdirectory(examples)

if(DISPATCH_BUILD_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()

install(DIRECTORY include DESTINATION include)
