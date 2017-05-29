function(dispatch_add_executable executable_name filename)
    add_executable(${executable_name} ${filename})
    set_property(TARGET ${executable_name} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${executable_name} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_compile_options(${executable_name} PUBLIC "-std=c++1z")

    # TODO XXX
    target_compile_options(${executable_name} PUBLIC "-DDISPATCH_USE_UDL")
    # clang only
    target_compile_options(${executable_name} PUBLIC "-fcolor-diagnostics;-Wall;-Wextra;-Werror")
endfunction()
