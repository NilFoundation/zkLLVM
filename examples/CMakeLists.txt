set(INPUTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inputs)

#generate assignment table and circuit
function(assign_ir)
    list(POP_FRONT ARGV target)
    list(POP_FRONT ARGV binary_name)
    list(POP_FRONT ARGV input)
    list(POP_FRONT ARGV private_input)
    list(POP_FRONT ARGV curve_type)
    list(POP_FRONT ARGV arithmetization)
    if(NOT arithmetization STREQUAL "none")
        set(witness_cols ${arithmetization})
        list(POP_FRONT ARGV public_input_cols)
        list(POP_FRONT ARGV component_constant_cols)
        list(POP_FRONT ARGV lookup_constant_cols)
        list(POP_FRONT ARGV component_selector_cols)
        list(POP_FRONT ARGV lookup_selector_cols)
        set(arithmetization_flag --column-sizes)
        set(arithmetization_amount ${witness_cols} ${public_input_cols} ${component_constant_cols} ${lookup_constant_cols} ${component_selector_cols} ${lookup_selector_cols})
    endif()
    list(POP_FRONT ARGV max_num_provers_amount)

    if(NOT max_num_provers_amount EQUAL 0)
        set(max_num_provers_flag --max-num-provers)
        set(max_num_provers_amount ${max_num_provers_amount})
    endif()

    if(NOT private_input STREQUAL "none")
        set(minus_p -p)
        set(private_input_string ${INPUTS_DIR}/${private_input})
    endif()

    add_custom_target(${target}_generate_crct
            COMMAND $<TARGET_FILE:assigner>
            -b ${binary_name}
            -c circuit_${target}.crct
            -t assignment_${target}.tbl
            -e ${curve_type}
            --generate-type circuit
            ${max_num_provers_flag} ${max_num_provers_amount}
            ${arithmetization_flag} ${arithmetization_amount}
            DEPENDS ${target} $<TARGET_FILE:assigner>
            COMMAND_EXPAND_LISTS
            VERBATIM)

    add_custom_target(${target}_generate_both
        COMMAND $<TARGET_FILE:assigner>
            -b ${binary_name}
            -i ${INPUTS_DIR}/${input}
            ${minus_p} ${private_input_string}
            -c circuit_${target}.crct
            -t assignment_${target}.tbl -e ${curve_type} --check
            --generate-type circuit-assignment
            ${max_num_provers_flag} ${max_num_provers_amount}
            ${arithmetization_flag} ${arithmetization_amount}
        DEPENDS ${target} ${INPUTS_DIR}/${input} $<TARGET_FILE:assigner>
        COMMAND_EXPAND_LISTS
        VERBATIM)

    add_custom_target(${target}_generate_both_no_check
        COMMAND $<TARGET_FILE:assigner>
            -b ${binary_name}
            -i ${INPUTS_DIR}/${input}
            ${minus_p} ${private_input_string}
            -c circuit_${target}.crct
            -t assignment_${target}.tbl -e ${curve_type}
            --generate-type circuit-assignment
            ${max_num_provers_flag} ${max_num_provers_amount}
            ${arithmetization_flag} ${arithmetization_amount}
        DEPENDS ${target} ${INPUTS_DIR}/${input} $<TARGET_FILE:assigner>
        COMMAND_EXPAND_LISTS
        VERBATIM)

    add_custom_target(${target}_generate_tbl
        COMMAND $<TARGET_FILE:assigner>
            -b ${binary_name}
            -i ${INPUTS_DIR}/${input}
            ${minus_p} ${private_input_string}
            -t assignment_${target}.tbl -e ${curve_type} --check
            --generate-type assignment
            ${max_num_provers_flag} ${max_num_provers_amount}
            ${arithmetization_flag} ${arithmetization_amount}
        DEPENDS ${target} ${INPUTS_DIR}/${input} $<TARGET_FILE:assigner>
        COMMAND_EXPAND_LISTS
        VERBATIM)

    add_custom_target(${target}_generate_tbl_fast
        COMMAND $<TARGET_FILE:assigner>
            -b ${binary_name}
            -i ${INPUTS_DIR}/${input}
            ${minus_p} ${private_input_string}
            -c circuit_${target}.crct
            -t assignment_${target}.tbl -e ${curve_type}
            --generate-type assignment-fast
            ${max_num_provers_flag} ${max_num_provers_amount}
            ${arithmetization_flag} ${arithmetization_amount}
        DEPENDS ${target} ${INPUTS_DIR}/${input} $<TARGET_FILE:assigner>
        COMMAND_EXPAND_LISTS
        VERBATIM)

    add_custom_target(${target}_generate_tbl_fast_depends_on_crct
        COMMAND $<TARGET_FILE:assigner>
            -b ${binary_name}
            -i ${INPUTS_DIR}/${input}
            ${minus_p} ${private_input_string}
            -c circuit_${target}.crct
            -t assignment_${target}.tbl -e ${curve_type}
            --generate-type assignment-fast
            ${max_num_provers_flag} ${max_num_provers_amount}
            ${arithmetization_flag} ${arithmetization_amount}
        DEPENDS ${target} ${INPUTS_DIR}/${input} ${target}_generate_crct $<TARGET_FILE:assigner>
        COMMAND_EXPAND_LISTS
        VERBATIM)

        add_custom_target(${target}_generate_tbl_no_check
        COMMAND $<TARGET_FILE:assigner>
            -b ${binary_name}
            -i ${INPUTS_DIR}/${input}
            ${minus_p} ${private_input_string}
            -t assignment_${target}.tbl -e ${curve_type}
            --generate-type assignment
            ${max_num_provers_flag} ${max_num_provers_amount}
            ${arithmetization_flag} ${arithmetization_amount}
        DEPENDS ${target} ${INPUTS_DIR}/${input} $<TARGET_FILE:assigner>
        COMMAND_EXPAND_LISTS
        VERBATIM)

    add_custom_target(${target}_estimate_size
        COMMAND $<TARGET_FILE:assigner>
            -b ${binary_name}
            -e ${curve_type}
            --generate-type size_estimation
            ${max_num_provers_flag} ${max_num_provers_amount}
            ${arithmetization_flag} ${arithmetization_amount}
        DEPENDS ${target} $<TARGET_FILE:assigner>
        COMMAND_EXPAND_LISTS
        VERBATIM)

    add_custom_target(${target}_prepare_input
        COMMAND $<TARGET_FILE:assigner>
            -b ${binary_name}
            -i ${INPUTS_DIR}/${input}
            --generate-type public-input-column
            --input-column ${target}_input_column.inp
            -e ${curve_type}
            ${arithmetization_flag} ${arithmetization_amount}
        DEPENDS ${target} ${INPUTS_DIR}/${input}
        COMMAND_EXPAND_LISTS
        VERBATIM)
endfunction()

function(gen_proof target curve_type provers_amount tbl_gen_speed)
    if(provers_amount EQUAL 0)
        gen_single_proof(${target} ${curve_type} 0 ${tbl_gen_speed})
    else()
        add_custom_target(${target}_prove)

        foreach(prover_num RANGE 1 ${provers_amount})
            math(EXPR prover_num_minus_1 "${prover_num} - 1")
            gen_single_proof(${target} ${curve_type} ${prover_num} ${tbl_gen_speed})
            add_dependencies(${target}_prove ${target}_prove${prover_num_minus_1})
        endforeach()

        add_custom_target(${target}_copy_input_for_proof
            COMMAND ${CMAKE_COMMAND} -E make_directory transpiler_output_${target}
            COMMAND ${CMAKE_COMMAND} -E copy ${target}_input_column.inp transpiler_output_${target}/public_input.inp
            DEPENDS ${target}_prepare_input)

        add_dependencies(${target}_prove ${target}_copy_input_for_proof)

    endif()
endfunction()


function(gen_single_proof target curve_type provers_amount tbl_gen_speed)
    if(NOT provers_amount EQUAL 0)
        set(multi_prover_flag --multi-prover)
        math(EXPR prover_num "${provers_amount} - 1")
    else()
        set(prover_num "")
    endif()

    set (depends_tbl ${target}_generate_both)
    if (tbl_gen_speed STREQUAL "fast")
        set (depends_tbl ${target}_generate_tbl_fast_depends_on_crct)
    endif()

    add_custom_target(${target}_prove${prover_num}
        COMMAND $<TARGET_FILE:transpiler> -m gen-test-proof
            -c circuit_${target}.crct${prover_num}
            -t assignment_${target}.tbl${prover_num}
            -o transpiler_output_${target}${prover_num}
            -e ${curve_type}
            ${multi_prover_flag}
            DEPENDS ${depends_tbl} $<TARGET_FILE:transpiler>
        COMMAND_EXPAND_LISTS
        VERBATIM)
endfunction()

function(gen_recursive_proof target curve_type provers_amount)
    if(NOT provers_amount EQUAL 0)
        math(EXPR prover_num "${provers_amount} - 1")
    else()
        set(prover_num "")
    endif()

    add_custom_target(${target}_gen_json_proof${prover_num}
        COMMAND ${CMAKE_COMMAND} -E make_directory recursive_${target}/${prover_num}
        COMMAND $<TARGET_FILE:recursive_gen> -m gen-input
            -c circuit_${target}.crct${prover_num}
            -t assignment_${target}.tbl${prover_num}
            -o recursive_${target}/${prover_num}
            -e ${curve_type}
            ${multi_prover_flag}
            DEPENDS ${target}_generate_crct ${target}_generate_tbl $<TARGET_FILE:transpiler>
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
    add_dependencies(${target}_gen_json_proofs ${target}_gen_json_proof${prover_num})
endfunction()

function(gen_recursive_proofs target curve_type provers_amount)
    if(provers_amount EQUAL 0)
        add_custom_target(${target}_gen_json_proofs)
        gen_recursive_proof(${target} ${curve_type} 0)
        add_dependencies(${target}_recursion ${target}_gen_json_proofs)
    else()
        add_custom_target(${target}_gen_json_proofs
            COMMAND ${CMAKE_COMMAND} -E make_directory recursive_${target}
        )
        foreach(prover_num RANGE 1 ${provers_amount})
            gen_recursive_proof(${target} ${curve_type} ${prover_num})
        endforeach()
        add_dependencies(${target}_recursion ${target}_gen_json_proofs)
    endif()
endfunction()

function(gen_recursive_verifier target curve_type provers_amount)
    if(NOT provers_amount EQUAL 0)
        math(EXPR prover_num "${provers_amount} - 1")
    else()
        set(prover_num "")
    endif()

    add_custom_target(${target}_gen_recursive_verifier${prover_num}
        COMMAND ${CMAKE_COMMAND} -E make_directory recursive_${target}/${prover_num}
        COMMAND $<TARGET_FILE:recursive_gen> -m gen-verifier
            -c circuit_${target}.crct${prover_num}
            -t assignment_${target}.tbl${prover_num}
            -o recursive_${target}/${prover_num}
            -e ${curve_type}
            ${multi_prover_flag}
            DEPENDS ${target}_generate_crct $<TARGET_FILE:transpiler>
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
    add_dependencies(${target}_gen_recursive_verifiers ${target}_gen_recursive_verifier${prover_num})
endfunction()

function(gen_recursive_verifiers target curve_type provers_amount)
    if(provers_amount EQUAL 0)
        add_custom_target(${target}_gen_recursive_verifiers)
        gen_recursive_verifier(${target} ${curve_type} 0)
        add_dependencies(${target}_recursion ${target}_gen_recursive_verifiers)
    else()
        add_custom_target(${target}_gen_recursive_verifiers
            COMMAND ${CMAKE_COMMAND} -E make_directory recursive_${target}
        )
        foreach(prover_num RANGE 1 ${provers_amount})
            gen_recursive_verifier(${target} ${curve_type} ${prover_num})
        endforeach()
        add_dependencies(${target}_recursion ${target}_gen_recursive_verifiers)
    endif()
endfunction()

function( add_circuit_with_recursion target )
    set(prefix ARG)
    set(name ${target}_build_placeholder_verifier${prover_num})
    set(noValues "")
    set(singleValues)
    set(multiValues SOURCES INCLUDE_DIRECTORIES LINK_LIBRARIES COMPILER_OPTIONS)
    cmake_parse_arguments(${prefix}
                          "${noValues}"
                          "${singleValues}"
                          "${multiValues}"
                          ${ARGN})

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "SOURCES for ${name} circuit was not defined")
    endif()

    foreach(source ${ARG_SOURCES})
        if(NOT IS_ABSOLUTE ${source})
            list(APPEND CIRCUIT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
        else()
            list(APPEND CIRCUIT_SOURCES "${source}")
        endif()
    endforeach()
    list(REMOVE_DUPLICATES CIRCUIT_SOURCES)

    foreach(ITR ${CIRCUIT_SOURCES})
        if(NOT EXISTS ${ITR})
            message(SEND_ERROR "Cannot find circuit source file: ${source}")
        endif()
    endforeach()

    set(INCLUDE_DIRS_LIST "")
    # Collect include directories from dependencies first
    foreach(lib ${ARG_LINK_LIBRARIES})
        get_target_property(lib_include_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
        foreach(dir ${lib_include_dirs})
            list(APPEND INCLUDE_DIRS_LIST "-I${dir}")
        endforeach()
    endforeach()
    # Add passed include directories
    foreach(include_dir ${ARG_INCLUDE_DIRECTORIES})
        if(NOT IS_ABSOLUTE ${include_dir})
            set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/${include_dir}")
        endif()
        list(APPEND INCLUDE_DIRS_LIST "-I${include_dir}")
    endforeach()
    if (ZKLLVM_DEV_ENVIRONMENT)
        list(APPEND INCLUDE_DIRS_LIST -I${CMAKE_SOURCE_DIR}/libs/stdlib/libcpp -I${CMAKE_SOURCE_DIR}/libs/circifier/clang/lib/Headers -I${CMAKE_SOURCE_DIR}/libs/stdlib/libc/include)
    endif()
    list(REMOVE_DUPLICATES INCLUDE_DIRS_LIST)

    if (NOT ${CIRCUIT_BINARY_OUTPUT})
        set(link_options "-S")
    endif()

    if (ZKLLVM_DEV_ENVIRONMENT)
        set(CLANG $<TARGET_FILE:clang>)
        set(LINKER $<TARGET_FILE:llvm-link>)
    else()
        set(CLANG clang-zkllvm)
        set(LINKER llvm-link-zkllvm)
    endif()

    # Compile sources
    set(compiler_outputs "")
    add_custom_target(${name}_compile_sources)
    foreach(source ${CIRCUIT_SOURCES})
        get_filename_component(source_base_name ${source} NAME)
        add_custom_target(${name}_${source_base_name}_ll
            COMMAND $<TARGET_FILE:recursive_gen> -m gen-input
            -c circuit_${target}.crct${prover_num}
            -t assignment_${target}.tbl${prover_num}
            -o recursive_${target}/${prover_num}
            -e ${curve_type}
            ${multi_prover_flag}
            DEPENDS ${target}_generate_crct ${target}_generate_tbl $<TARGET_FILE:recursive_gen>

            COMMAND $<TARGET_FILE:recursive_gen> -m gen-verifier
            -c circuit_${target}.crct${prover_num}
            -t assignment_${target}.tbl${prover_num}
            -o recursive_${target}/${prover_num}
            -e ${curve_type}
            ${multi_prover_flag}
            DEPENDS ${target}_generate_crct ${target}_generate_tbl $<TARGET_FILE:recursive_gen>

            COMMAND ${CLANG} -target assigner -Xclang -fpreserve-vec3-type -Werror=unknown-attributes -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION
            -D__ZKLLVM__ ${INCLUDE_DIRS_LIST} -emit-llvm -O1 -S ${ARG_COMPILER_OPTIONS}  -o ${name}_${source_base_name}.ll ${source}

            VERBATIM COMMAND_EXPAND_LISTS
            SOURCES ${source}
        )
        add_dependencies(${name}_compile_sources ${name}_${source_base_name}_ll)
        list(APPEND compiler_outputs "${name}_${source_base_name}.ll")
    endforeach()

    if(NOT ${prover_num} EQUAL "")
        set(sources_path ${CMAKE_CURRENT_BINARY_DIR}/recursive_${target}/${prover_num}/)
    else()
        set(sources_path ${CMAKE_CURRENT_BINARY_DIR}/recursive_${target}/)
    endif()

    # Link sources
    add_custom_target(${name}
                    COMMAND ${LINKER} ${link_options} -o ${name}.ll ${compiler_outputs}
                    COMMAND $<TARGET_FILE:assigner>
                      -b ${CMAKE_CURRENT_BINARY_DIR}/${target}_build_placeholder_verifier${prover_num}.ll
                      -i ${sources_path}placeholder_verifier.inp
                      -c ${sources_path}/circuit_${target}.crct
                      -t ${sources_path}/assignment_${target}.tbl -e ${curve_type} --check
                      --generate-type circuit-assignment
                    DEPENDS ${name}_compile_sources
                      VERBATIM COMMAND_EXPAND_LISTS)
    set_target_properties(${name} PROPERTIES OUTPUT_NAME ${name}.ll)
endfunction()

function(assign_recursive_verifier target curve_type provers_amount)
    if(NOT provers_amount EQUAL 0)
        math(EXPR prover_num "${provers_amount} - 1")
        set(sources_path ${CMAKE_CURRENT_BINARY_DIR}/recursive_${target}/${prover_num}/)
    else()
        set(prover_num "")
        set(sources_path ${CMAKE_CURRENT_BINARY_DIR}/recursive_${target}/)
    endif()

    file(MAKE_DIRECTORY ${sources_path})
    if( NOT EXISTS ${sources_path}/placeholder_verifier.cpp )
        FILE(WRITE ${sources_path}/placeholder_verifier.cpp "Hello, world")
    endif()

    add_circuit_with_recursion(${target}
        PROVER_NUM ${prover_num}
        SOURCES ${sources_path}placeholder_verifier.cpp
        COMPILER_OPTIONS ${ARG_COMPILER_OPTIONS}

        LINK_LIBRARIES
        crypto3::algebra
        crypto3::block
        crypto3::blueprint
        crypto3::codec
        crypto3::containers
        crypto3::hash
        crypto3::kdf
        crypto3::mac
        marshalling::core
        marshalling::crypto3_algebra
        marshalling::crypto3_multiprecision
        marshalling::crypto3_zk
        crypto3::math
        crypto3::modes
        crypto3::multiprecision
        crypto3::passhash
        crypto3::pbkdf
        crypto3::threshold
        crypto3::pkpad
        crypto3::pubkey
        crypto3::random
        crypto3::stream
        crypto3::vdf
        crypto3::zk

        ${Boost_LIBRARIES}
    )
    add_dependencies(${target}_assign_placeholder_verifiers ${target}_build_placeholder_verifier${prover_num})
endfunction()

function(assign_recursive_verifiers target curve_type provers_amount)
    add_custom_target( ${target}_assign_placeholder_verifiers
        COMMAND ${CMAKE_COMMAND} -E make_directory recursive_${target}
    )
    gen_recursive_proofs(${target} ${ARG_CURVE_TYPE} ${ARG_MAX_NUM_PROVERS})
    gen_recursive_verifiers(${target} ${ARG_CURVE_TYPE} ${ARG_MAX_NUM_PROVERS})
    if(provers_amount EQUAL 0)
        assign_recursive_verifier(${target} ${curve_type} 0)
    else()
        foreach(prover_num RANGE 1 ${provers_amount})
            assign_recursive_verifier(${target} ${curve_type} ${prover_num})
        endforeach()
    endif()
endfunction()

function(gen_evm_verifier target curve_type provers_amount)

    if(provers_amount EQUAL 0)
    gen_single_evm_verifier(${target} ${curve_type} 0)
    else()
        add_custom_target(${target}_evm_verifier)

        foreach(prover_num RANGE 1 ${provers_amount})
            math(EXPR prover_num_minus_1 "${prover_num} - 1")
            gen_single_evm_verifier(${target} ${curve_type} ${prover_num})
            add_dependencies(${target}_evm_verifier ${target}_evm_verifier${prover_num_minus_1})
        endforeach()

    endif()

    add_custom_target(${target}_copy_input_for_evm_verifier
            COMMAND ${CMAKE_COMMAND} -E make_directory transpiler_output_${target}
            COMMAND ${CMAKE_COMMAND} -E copy ${target}_input_column.inp transpiler_output_${target}/public_input.inp
            DEPENDS ${target}_prepare_input)
    add_dependencies(${target}_evm_verifier ${target}_copy_input_for_evm_verifier)
endfunction()

function(gen_single_evm_verifier target curve_type provers_amount)
    if(NOT provers_amount EQUAL 0)
        set(multi_prover_flag --multi-prover)
        math(EXPR prover_num "${provers_amount} - 1")
    else()
        set(prover_num "")
    endif()

    add_custom_target(${target}_evm_verifier${prover_num}
        COMMAND $<TARGET_FILE:transpiler> -m gen-evm-verifier
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
