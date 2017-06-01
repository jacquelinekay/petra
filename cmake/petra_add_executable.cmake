function(petra_add_executable executable_name filename)
    add_executable(${executable_name} ${filename})
    set_property(TARGET ${executable_name} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${executable_name} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_compile_options(${executable_name} PUBLIC "-std=c++1z")
    target_include_directories(${executable_name} PUBLIC ${CMAKE_SOURCE_DIR}/include)

    target_compile_options(${executable_name} PUBLIC "-Wall;-Wextra;-Werror")

    if (PETRA_USE_UDL)
        target_compile_options(${executable_name} PUBLIC
            "-Wno-gnu-string-literal-operator-template;-DPETRA_USE_UDL")
    endif()
endfunction()
