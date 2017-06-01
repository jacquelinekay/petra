include(petra_add_executable)

add_custom_target(examples COMMENT "Build example programs.")

set(example_names "callback_table;map;switch_table;string_hash")

foreach(_name IN LISTS example_names)
    petra_add_executable(${_name} ${_name}.cpp)
    add_dependencies(examples ${_name})
endforeach()
