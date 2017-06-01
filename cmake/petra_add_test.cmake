function(petra_add_test test_name filename)
    petra_add_executable(${test_name}_test_executable ${filename})
    add_test(NAME ${test_name}_test
             COMMAND ${test_name}_test_executable)
endfunction()
