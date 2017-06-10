# Copyright Jacqueline Kay 2017
# Distributed under the MIT License.
# See accompanying LICENSE.md or https://opensource.org/licenses/MIT

include(petra_add_executable)

add_custom_target(examples COMMENT "Build example programs.")

set(example_names "callback_table;map;sequential_table;sequence_map;switch_table;string_hash")

foreach(_name IN LISTS example_names)
    petra_add_executable(${_name} ${_name}.cpp)
    add_dependencies(examples ${_name})
endforeach()
