# Copyright Jacqueline Kay 2017
# Distributed under the MIT License.
# See accompanying LICENSE.md or https://opensource.org/licenses/MIT

function(petra_add_test test_name filename)
    petra_add_executable(${test_name}_test_executable ${filename})
    add_test(NAME ${test_name}_test
             COMMAND ${test_name}_test_executable)
endfunction()
