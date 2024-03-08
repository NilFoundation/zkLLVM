add_custom_target(compile_cpp_examples)
add_custom_target(cpp_examples_generate_tbl)
add_custom_target(cpp_examples_generate_crct)
add_custom_target(cpp_examples_generate_both)
add_custom_target(prove_cpp_examples)
add_custom_target(cpp_examples_estimate_size)

function(add_example_without_proving example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues INPUT PRIVATE_INPUT CURVE_TYPE MAX_NUM_PROVERS)
    set(multiValues SOURCES COMPILER_OPTIONS ARITHMETIZARION)
    cmake_parse_arguments(${prefix}
                        "${noValues}"
                        "${singleValues}"
                        "${multiValues}"
                        ${ARGN})

    add_circuit(${example_target}
                SOURCES ${ARG_SOURCES}
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

                ${Boost_LIBRARIES})

    set(binary_name ${example_target}.ll)

    add_dependencies(compile_cpp_examples ${example_target})

    if(NOT DEFINED ARG_PRIVATE_INPUT)
        set(ARG_PRIVATE_INPUT "none")
    endif()

    if(NOT DEFINED ARG_MAX_NUM_PROVERS)
        set(ARG_MAX_NUM_PROVERS 0)
    endif()

    if(NOT DEFINED ARG_ARITHMETIZARION)
        set(ARG_ARITHMETIZARION "none")
    endif()

    assign_ir(${example_target} ${binary_name} ${ARG_INPUT} ${ARG_PRIVATE_INPUT} ${ARG_CURVE_TYPE} ${ARG_ARITHMETIZARION} ${ARG_MAX_NUM_PROVERS})

    add_dependencies(cpp_examples_generate_tbl ${example_target}_generate_tbl)
    add_dependencies(cpp_examples_generate_crct ${example_target}_generate_crct)
    add_dependencies(cpp_examples_generate_both ${example_target}_generate_both)
    add_dependencies(cpp_examples_estimate_size ${example_target}_estimate_size)
endfunction()

function(add_example_with_proving example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues INPUT PRIVATE_INPUT CURVE_TYPE MAX_NUM_PROVERS)
    set(multiValues SOURCES)
    cmake_parse_arguments(${prefix}
                        "${noValues}"
                        "${singleValues}"
                        "${multiValues}"
                        ${ARGN})

    add_example_without_proving(${example_target} ${ARGN})

    if(NOT DEFINED ARG_MAX_NUM_PROVERS)
        set(ARG_MAX_NUM_PROVERS 0)
    endif()

    gen_proof(${example_target} ${ARG_CURVE_TYPE} ${ARG_MAX_NUM_PROVERS})
    add_dependencies(prove_cpp_examples ${example_target}_prove)

    if(GENERATE_EVM_VERIFIER)
        gen_evm_verifier(${example_target} ${ARG_CURVE_TYPE} ${ARG_MAX_NUM_PROVERS})
        add_dependencies(prove_cpp_examples ${example_target}_evm_verifier)
    endif()

endfunction()

add_example_without_proving(placeholder_verifier_cpp_example SOURCES placeholder_verifier.cpp INPUT placeholder_verifier.inp CURVE_TYPE pallas)
add_example_with_proving(arithmetics_cpp_example SOURCES arithmetics.cpp INPUT arithmetics.inp CURVE_TYPE pallas)
add_example_with_proving(integer_arithmetics_cpp_example SOURCES integer_arithmetics.cpp INPUT integer_arithmetics.inp CURVE_TYPE pallas)

add_example_with_proving(poseidon_cpp_example SOURCES poseidon.cpp INPUT poseidon.inp CURVE_TYPE pallas)
add_example_with_proving(merkle_tree_poseidon_cpp_example SOURCES merkle_tree_poseidon.cpp INPUT merkle_tree_poseidon.inp CURVE_TYPE pallas)
add_example_with_proving(merkle_poseidon_1prover_cpp_example SOURCES merkle_poseidon_1prover.cpp INPUT merkle_poseidon_1prover_public.inp PRIVATE_INPUT merkle_poseidon_1prover_private.inp CURVE_TYPE pallas)
add_example_with_proving(merkle_poseidon_2provers_cpp_example SOURCES merkle_poseidon_2provers.cpp INPUT merkle_poseidon_2provers_public.inp PRIVATE_INPUT merkle_poseidon_2provers_private.inp CURVE_TYPE pallas MAX_NUM_PROVERS 2)
add_example_with_proving(merkle_poseidon_3provers_cpp_example SOURCES merkle_poseidon_3provers.cpp INPUT merkle_poseidon_3provers_public.inp PRIVATE_INPUT merkle_poseidon_3provers_private.inp CURVE_TYPE pallas MAX_NUM_PROVERS 3)
add_example_with_proving(merkle_poseidon_5provers_cpp_example SOURCES merkle_poseidon_5provers.cpp INPUT merkle_poseidon_5provers_public.inp PRIVATE_INPUT merkle_poseidon_5provers_private.inp CURVE_TYPE pallas MAX_NUM_PROVERS 5)
add_example_with_proving(merkle_poseidon_21prover_3layers_cpp_example SOURCES merkle_poseidon_21prover_3layers.cpp INPUT merkle_poseidon_21prover_3layers_public.inp PRIVATE_INPUT merkle_poseidon_21prover_3layers_private.inp CURVE_TYPE pallas MAX_NUM_PROVERS 21)

add_example_without_proving(merkle_tree_sha2_256_cpp_example SOURCES merkle_tree_sha2_256.cpp INPUT merkle_tree_sha2_256.inp CURVE_TYPE pallas)
add_example_without_proving(validate_merkle_path_sha2_256_cpp_example SOURCES validate_merkle_path_sha2_256.cpp INPUT validate_merkle_path_sha2_256.inp CURVE_TYPE pallas)
add_example_without_proving(sha256_cpp_example SOURCES sha2_256.cpp INPUT sha2_256.inp CURVE_TYPE pallas)
add_example_with_proving(balance_cpp_example SOURCES balance.cpp INPUT balance.inp CURVE_TYPE pallas)
add_example_with_proving(memory_cpp_example SOURCES memory.cpp INPUT memory.inp CURVE_TYPE pallas)
add_example_with_proving(polynomial_cpp_example SOURCES polynomial.cpp INPUT polynomial.inp COMPILER_OPTIONS -std=c++20 CURVE_TYPE pallas)
add_example_with_proving(pallas_curve_addition_cpp_example
           SOURCES pallas_curve_examples/pallas_curve_add.cpp
           INPUT pallas_curve_examples/pallas_curve_add.inp
           CURVE_TYPE pallas)
add_example_with_proving(pallas_curve_multiplication_cpp_example
           SOURCES pallas_curve_examples/pallas_curve_mul.cpp
           INPUT pallas_curve_examples/pallas_curve_mul.inp
           CURVE_TYPE pallas)
add_example_with_proving(ed25519_curve_add_cpp SOURCES ed25519_curve_add.cpp INPUT ed25519_curve_add.inp CURVE_TYPE pallas)
add_example_with_proving(ed25519_curve_mul_cpp SOURCES ed25519_curve_mul.cpp INPUT ed25519_curve_mul.inp CURVE_TYPE pallas)
add_example_with_proving(ed25519_field_add_cpp SOURCES ed25519_field_add.cpp INPUT ed25519_field_add.inp CURVE_TYPE pallas)
#add_example_with_proving(strlen_cpp_example SOURCES strlen.cpp INPUT strlen.inp CURVE_TYPE pallas)
add_example_with_proving(sha2_512_cpp SOURCES sha2_512.cpp INPUT sha2_512.inp CURVE_TYPE pallas)

add_example_with_proving(uint_remainder_cpp SOURCES bit_operations/uint_remainder.cpp INPUT bit_operations/uint_remainder.inp CURVE_TYPE pallas)
add_example_with_proving(uint_division_cpp SOURCES bit_operations/uint_division.cpp INPUT bit_operations/uint_division.inp CURVE_TYPE pallas)
add_example_with_proving(uint_shift_left SOURCES bit_operations/shift_left.cpp INPUT bit_operations/shift_left.inp CURVE_TYPE pallas)
add_example_with_proving(uint_shift_right SOURCES bit_operations/shift_right.cpp INPUT bit_operations/shift_right.inp CURVE_TYPE pallas)
add_example_with_proving(uint_bit_decomposition SOURCES bit_operations/bit_decomposition.cpp INPUT bit_operations/bit_decomposition.inp CURVE_TYPE pallas)
add_example_with_proving(uint_bit_composition SOURCES bit_operations/bit_composition.cpp INPUT bit_operations/bit_composition.inp CURVE_TYPE pallas)

add_example_with_proving(compare_neq_cpp SOURCES compare/neq.cpp INPUT compare/neq.inp CURVE_TYPE pallas)
add_example_with_proving(compare_eq_cpp SOURCES compare/eq.cpp INPUT compare/eq.inp CURVE_TYPE pallas)
add_example_with_proving(compare_grequal_cpp SOURCES compare/grequal.cpp INPUT compare/grequal.inp CURVE_TYPE pallas)
add_example_with_proving(compare_lequal_cpp SOURCES compare/lequal.cpp INPUT compare/lequal.inp CURVE_TYPE pallas)
add_example_with_proving(compare_greater_than_cpp SOURCES compare/greater_than.cpp INPUT compare/greater_than.inp CURVE_TYPE pallas)
add_example_with_proving(compare_less_than_cpp SOURCES compare/less_than.cpp INPUT compare/less_than.inp CURVE_TYPE pallas)

#Balances tree example is too big to be proven in a reasonable time
add_example_without_proving(balances_tree_cpp_example SOURCES balances_tree.cpp INPUT balances_tree_public.inp PRIVATE_INPUT balances_tree_private.inp CURVE_TYPE pallas)

add_example_without_proving(eddsa_signature_verification_cpp SOURCES eddsa_signature_verification.cpp INPUT eddsa_signature_verification.inp CURVE_TYPE pallas)
add_example_without_proving(zkbridge_cpp SOURCES zkbridge.cpp INPUT zkbridge.inp CURVE_TYPE pallas ARITHMETIZARION 15 1 5 30 50 6)

add_example_with_proving(private_input_cpp SOURCES private_input.cpp INPUT private_input_public.inp PRIVATE_INPUT private_input_private.inp CURVE_TYPE pallas)
add_example_with_proving(private_input_array_cpp SOURCES private_input_array.cpp INPUT private_input_array_public.inp PRIVATE_INPUT private_input_array_private.inp CURVE_TYPE pallas)

add_example_with_proving(fri_lin_inter SOURCES placeholder_prover/fri_lin_inter.cpp INPUT placeholder_prover/fri_lin_inter.inp CURVE_TYPE pallas)
add_example_with_proving(fri_array_swap SOURCES placeholder_prover/fri_array_swap.cpp INPUT placeholder_prover/fri_array_swap.inp CURVE_TYPE pallas)
add_example_with_proving(fri_array_swap_intrinsic SOURCES placeholder_prover/fri_array_swap_intrinsic.cpp INPUT placeholder_prover/fri_array_swap_intrinsic.inp CURVE_TYPE pallas)
add_example_with_proving(fri_cosets SOURCES placeholder_prover/fri_cosets.cpp INPUT placeholder_prover/fri_cosets.inp CURVE_TYPE pallas)
add_example_with_proving(gate_argument_verifier SOURCES placeholder_prover/gate_argument_verifier.cpp INPUT placeholder_prover/gate_argument_verifier.inp CURVE_TYPE pallas)
add_example_with_proving(permutation_argument_verifier SOURCES placeholder_prover/permutation_argument_verifier.cpp INPUT placeholder_prover/permutation_argument_verifier.inp CURVE_TYPE pallas)
add_example_with_proving(lookup_argument_verifier SOURCES placeholder_prover/lookup_argument_verifier.cpp INPUT placeholder_prover/lookup_argument_verifier.inp CURVE_TYPE pallas)

add_example_without_proving(bls12_381_field_add_cpp SOURCES bls12_381_field_add.cpp INPUT bls12_381_field_add.inp CURVE_TYPE bls12381)
add_example_without_proving(bls12_381_field_mul_cpp SOURCES bls12_381_field_mul.cpp INPUT bls12_381_field_mul.inp CURVE_TYPE bls12381)
add_example_without_proving(bls12_381_field_sub_cpp SOURCES bls12_381_field_sub.cpp INPUT bls12_381_field_sub.inp CURVE_TYPE bls12381)
add_example_without_proving(bls12_381_signature_verification_cpp SOURCES bls12_381_signature_verification.cpp INPUT bls12_381_signature_verification.inp CURVE_TYPE bls12381)
add_example_without_proving(bls12_381_signature_verification_aggregated_cpp SOURCES bls12_381_signature_verification_aggregated.cpp INPUT bls12_381_signature_verification_aggregated.inp CURVE_TYPE bls12381)
