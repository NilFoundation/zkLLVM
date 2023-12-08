set(INPUTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inputs)

function(assign_ir target binary_name input private_input curve_type)

    if(NOT private_input STREQUAL "")
        set (minus_p -p)
        set(private_input_string ${INPUTS_DIR}/${private_input})
    endif()

    add_custom_target(${target}_assign
        COMMAND $<TARGET_FILE:assigner>
            -b ${binary_name}
            -i ${INPUTS_DIR}/${input}
            ${minus_p} ${private_input_string}
            -c circuit_${target}.crct
            -t assignment_${target}.tbl -e ${curve_type} --check
        DEPENDS ${target} ${INPUTS_DIR}/${input} $<TARGET_FILE:assigner>
        COMMAND_EXPAND_LISTS
        VERBATIM)
endfunction()

function(gen_proof target input curve_type)
    add_custom_target(${target}_prove
        COMMAND $<TARGET_FILE:transpiler> -m gen-test-proof
            -i ${INPUTS_DIR}/${input}
            -c circuit_${target}.crct
            -t assignment_${target}.tbl
            -o transpiler_output_${target}
            -e ${curve_type}
        DEPENDS ${target}_assign $<TARGET_FILE:transpiler>
        COMMAND_EXPAND_LISTS
        VERBATIM)
endfunction()

function(gen_evm_verifier target input curve_type)
    add_custom_target(${target}_evm_verifier
        COMMAND $<TARGET_FILE:transpiler> -m gen-evm-verifier
            -i ${INPUTS_DIR}/${input}
            -c circuit_${target}.crct
            -t assignment_${target}.tbl
            -o transpiler_output_${target}
            -e ${curve_type}
            --optimize-gates
        DEPENDS ${target}_assign ${target}_prove $<TARGET_FILE:transpiler>
        COMMAND_EXPAND_LISTS
        VERBATIM)
endfunction()

add_subdirectory(cpp)
add_subdirectory(rust)
