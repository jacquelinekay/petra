# Copyright Jacqueline Kay 2017
# Distributed under the MIT License.
# See accompanying LICENSE.md or https://opensource.org/licenses/MIT

function(petra_add_benchmark benchmark_name N M T output_prefix)
    set(output "${output_prefix}_${N}_${T}.cpp")
    set(full_name "${benchmark_name}_${N}_${T}")
    add_custom_command(
        OUTPUT ${full_name}_generated_ ${output}
        COMMAND python3 ${CMAKE_SOURCE_DIR}/benchmarks/generate_benchmark.py
            ${N} ${M} ${T}
            --in_filename ${CMAKE_SOURCE_DIR}/benchmarks/string_petra.cpp.empy
            --out_filename ${output}
        COMMAND ${CMAKE_COMMAND} -E touch ${full_name}_generated_
        DEPENDS
        ${CMAKE_SOURCE_DIR}/benchmarks/generate_benchmark.py
        ${CMAKE_SOURCE_DIR}/benchmarks/string_petra.cpp.empy
    )
    add_custom_target(${full_name}_generated DEPENDS ${full_name}_generated_)

    add_executable(${full_name} ${output})
    add_dependencies(${full_name} ${full_name}_generated)
    target_compile_options(${full_name} PUBLIC "${ARGN}")

    add_custom_command(
        OUTPUT ${full_name}_run_
        COMMAND python3 ${CMAKE_SOURCE_DIR}/benchmarks/run_benchmarks.py
            ${CMAKE_BINARY_DIR}/${full_name} 100
            --output ${CMAKE_BINARY_DIR}/${full_name}_results.csv
        COMMAND ${CMAKE_COMMAND} -E touch ${full_name}_run_
        DEPENDS
        ${CMAKE_SOURCE_DIR}/benchmarks/run_benchmarks.py
        ${full_name}
    )
    add_custom_target(${full_name}_run DEPENDS ${full_name}_run_)
    add_dependencies(benchmarks ${full_name}_run)
endfunction()
