# Copyright Jacqueline Kay 2017
# Distributed under the MIT License.
# See accompanying LICENSE.md or https://opensource.org/licenses/MIT

include(petra_add_test)

set(test_names "enum_map;sequence_map;sequential_table;switch_table")
if(NOT PETRA_USE_CPP14)
  list(APPEND test_names "callback_table;chd;linear_hash;map;string_map")
endif()

foreach(_name IN LISTS test_names)
    petra_add_test(${_name} ${_name}.cpp)
endforeach()
