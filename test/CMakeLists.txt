# Copyright Jacqueline Kay 2017
# Distributed under the MIT License.
# See accompanying LICENSE.md or https://opensource.org/licenses/MIT

include(petra_add_test)

set(test_names "callback_table;chd;map;linear_hash;sequential_table;switch_table")

foreach(_name IN LISTS test_names)
    petra_add_test(${_name} ${_name}.cpp)
endforeach()
