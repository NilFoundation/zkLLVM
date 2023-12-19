set(INPUTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inputs)

function(assign_ir target input private_input curve_type max_num_provers_amount)

    if(NOT max_num_provers_amount EQUAL 0)
        set(max_num_provers_flag --max-num-provers)
        set(max_num_provers_amount ${max_num_provers_amount})
    endif()

    if(NOT private_input STREQUAL "")
        set (minus_p -p)
        set(private_input_string ${INPUTS_DIR}/${private_input})
    endif()

    add_custom_target(${target}_assign
        COMMAND $<TARGET_FILE:assigner>
            -b ${target}.ll
            -i ${INPUTS_DIR}/${input}
            ${minus_p} ${private_input_string}
            -c circuit_${target}.crct
            -t assignment_${target}.tbl -e ${curve_type} --check
            ${max_num_provers_flag} ${max_num_provers_amount}
        DEPENDS ${target} ${INPUTS_DIR}/${input} $<TARGET_FILE:assigner>
        COMMAND_EXPAND_LISTS
        VERBATIM)
endfunction()

function(gen_proof target input curve_type provers_amount)

    if(provers_amount EQUAL 0)
        gen_single_proof(${target} ${input} ${curve_type} 0)
    else()
        add_custom_target(${target}_prove)

        foreach(prover_num RANGE 1 ${provers_amount})
            math(EXPR prover_num_minus_1 "${prover_num} - 1")
            gen_single_proof(${target} ${input} ${curve_type} ${prover_num})
            add_dependencies(${target}_prove ${target}_prove${prover_num_minus_1})
        endforeach()

    endif()
endfunction()

function(gen_single_proof target input curve_type provers_amount)

    if(NOT provers_amount EQUAL 0)
        set(multi_prover_flag --multi-prover)
        math(EXPR prover_num "${provers_amount} - 1")
    else()
        set(prover_num "")
    endif()

    add_custom_target(${target}_prove${prover_num}
        COMMAND $<TARGET_FILE:transpiler> -m gen-test-proof
            -i ${INPUTS_DIR}/${input}
            -c circuit_${target}.crct${prover_num}
            -t assignment_${target}.tbl${prover_num}
            -o transpiler_output_${target}${prover_num}
            -e ${curve_type}
            ${multi_prover_flag}
            DEPENDS ${target}_assign $<TARGET_FILE:transpiler>
        COMMAND_EXPAND_LISTS
        VERBATIM)
endfunction()

function(gen_evm_verifier target input curve_type provers_amount)

    if(provers_amount EQUAL 0)
    gen_single_evm_verifier(${target} ${input} ${curve_type} 0)
    else()
        add_custom_target(${target}_evm_verifier)

        foreach(prover_num RANGE 1 ${provers_amount})
            math(EXPR prover_num_minus_1 "${prover_num} - 1")
            gen_single_evm_verifier(${target} ${input} ${curve_type} ${prover_num})
            add_dependencies(${target}_evm_verifier ${target}_evm_verifier${prover_num_minus_1})
        endforeach()

    endif()
endfunction()

function(gen_single_evm_verifier target input curve_type provers_amount)

    if(NOT provers_amount EQUAL 0)
        set(multi_prover_flag --multi-prover)
        math(EXPR prover_num "${provers_amount} - 1")
    else()
        set(prover_num "")
    endif()

    add_custom_target(${target}_evm_verifier${prover_num}
        COMMAND $<TARGET_FILE:transpiler> -m gen-evm-verifier
            -i ${INPUTS_DIR}/${input}
            -c circuit_${target}.crct${prover_num}
            -t assignment_${target}.tbl${prover_num}
            -o transpiler_output_${target}${prover_num}
            -e ${curve_type}
            ${multi_prover_flag}
            --optimize-gates
        DEPENDS ${target}_assign ${target}_prove${prover_num} $<TARGET_FILE:transpiler>
        COMMAND_EXPAND_LISTS
        VERBATIM)
endfunction()

add_subdirectory(cpp)
add_subdirectory(rust)
