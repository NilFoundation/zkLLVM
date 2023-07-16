add_custom_target(circuit_cpp_examples)
add_custom_target(assign_cpp_examples)
add_custom_target(prove_cpp_examples)

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
    add_dependencies(circuit_cpp_examples "${example_target}")

    if (CIRCUIT_ASSEMBLY_OUTPUT)
        set(binary_name ${example_target}.ll)
    else()
        set(binary_name ${example_target}.bc)
    endif()

    assign_ir(${example_target} ${binary_name} ${ARG_INPUT})
    add_dependencies(assign_cpp_examples ${example_target}_assign)
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
    add_dependencies(prove_cpp_examples ${example_target}_prove)
endfunction()

add_example_with_proving(arithmetics_cpp_example SOURCES arithmetics.cpp INPUT arithmetics.inp)
add_example_with_proving(integer_arithmetics_cpp_example SOURCES integer_arithmetics.cpp INPUT integer_arithmetics.inp)
add_example_with_proving(poseidon_cpp_example SOURCES poseidon.cpp INPUT poseidon.inp)
add_example_with_proving(merkle_tree_poseidon_cpp_example SOURCES merkle_tree_poseidon.cpp INPUT merkle_tree_poseidon.inp)
add_example_with_proving(merkle_tree_sha2_256_cpp_example SOURCES merkle_tree_sha2_256.cpp INPUT merkle_tree_sha2_256.inp)
add_example_with_proving(sha256_cpp_example SOURCES sha2_256.cpp INPUT sha2_256.inp)
add_example_with_proving(balance_cpp_example SOURCES balance.cpp INPUT balance.inp)
add_example_with_proving(memory_cpp_example SOURCES memory.cpp INPUT memory.inp)
add_example_with_proving(polynomial_cpp_example SOURCES polynomial.cpp INPUT polynomial.inp)
add_example_with_proving(pallas_curve_addition_cpp_example
           SOURCES pallas_curve_examples/pallas_curve_add.cpp
           INPUT pallas_curve_examples/pallas_curve_add.inp)
add_example_with_proving(pallas_curve_multiplication_cpp_example
            SOURCES pallas_curve_examples/pallas_curve_mul.cpp
            INPUT pallas_curve_examples/pallas_curve_mul.inp)
#add_example_with_proving(pallas_curve_mul_add_cpp_example
#            SOURCES pallas_curve_examples/pallas_curve_mul_add.cpp
#            INPUT pallas_curve_examples/pallas_curve_mul_add.inp)
#add_example_with_proving(ed25519_curve_add_cpp SOURCES ed25519_curve_add.cpp INPUT ed25519_curve_add.inp)
add_example_with_proving(ed25519_field_add_cpp SOURCES ed25519_field_add.cpp INPUT ed25519_field_add.inp)
add_example_with_proving(strlen_cpp_example SOURCES strlen.cpp INPUT strlen.inp)

#Balances tree example is too big to be proven in a reasonable time
add_example_without_proving(balances_tree_cpp_example SOURCES balances_tree.cpp INPUT balances_tree.inp)