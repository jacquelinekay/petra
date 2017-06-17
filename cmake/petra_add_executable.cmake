# Copyright Jacqueline Kay 2017
# Distributed under the MIT License.
# See accompanying LICENSE.md or https://opensource.org/licenses/MIT

function(petra_add_executable executable_name filename)
    add_executable(${executable_name} ${filename})
    target_compile_options(${executable_name} PUBLIC "-std=${PETRA_CXX_STD}")
    target_include_directories(${executable_name} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    if(PETRA_USE_CPP14)
      target_include_directories(${executable_name} PUBLIC ${Hana_INCLUDE_DIR})
    endif()

    target_compile_options(${executable_name} PUBLIC "-Wall;-Wextra;-Werror")

    if(PETRA_USE_UDL)
        target_compile_options(${executable_name} PUBLIC "-DPETRA_USE_UDL")
        if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
            target_compile_options(${executable_name} PUBLIC
                "-Wno-gnu-string-literal-operator-template")

        endif()
    endif()
endfunction()
