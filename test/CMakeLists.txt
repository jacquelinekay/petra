include(dispatch_add_test)

set(test_names "callback_table;chd;map;linear_hash;sequential_table;switch_table")

foreach(_name IN LISTS test_names)
    dispatch_add_test(${_name} ${_name}.cpp)
endforeach()
