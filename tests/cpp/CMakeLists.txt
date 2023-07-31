add_custom_target(test_cpp_examples)


###
add_custom_target(circuit_cpp_tests)
add_custom_target(assign_cpp_tests)
add_custom_target(prove_cpp_tests)

function(add_example_without_proving example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues INPUT)
    set(multiValues SOURCES)
    cmake_parse_arguments(${prefix}
                        "${noValues}"
                        "${singleValues}"
                        "${multiValues}"
                        ${ARGN})
    add_circuit(${example_target}
                SOURCES ${ARG_SOURCES}

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

                ${Boost_LIBRARIES})
    add_dependencies(circuit_cpp_tests "${example_target}")

    if (CIRCUIT_ASSEMBLY_OUTPUT)
        set(binary_name ${example_target}.ll)
    else()
        set(binary_name ${example_target}.bc)
    endif()

    assign_ir(${example_target} ${binary_name} ${ARG_INPUT})
    add_dependencies(assign_cpp_tests ${example_target}_assign)
endfunction()

function(add_example_with_proving example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues INPUT)
    set(multiValues SOURCES)
    cmake_parse_arguments(${prefix}
                        "${noValues}"
                        "${singleValues}"
                        "${multiValues}"
                        ${ARGN})
    
    add_example_without_proving(${example_target} ${ARGN})

    gen_proof(${example_target} ${ARG_INPUT})
    add_dependencies(prove_cpp_tests ${example_target}_prove)
endfunction()

###


add_example_without_proving(native_pallas_field_add SOURCES algebra/fields/native_pallas_field_add.cpp INPUT algebra/fields/native_pallas_field_add.inp)
