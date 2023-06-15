add_custom_target(circuit_examples)
add_custom_target(run_examples)

function(add_example example_target)
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
    add_dependencies(circuit_examples "${example_target}")

    if (CIRCUIT_ASSEMBLY_OUTPUT)
        set(binary_name ${example_target}.ll)
    else()
        set(binary_name ${example_target}.bc)
    endif()
    add_custom_target(${example_target}_run
                      COMMAND $<TARGET_FILE:assigner> -b ${binary_name} -i ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_INPUT} -c circuit_${example_target}.crct -t assignment_${example_target}.tbl -e pallas
                      DEPENDS ${example_target} ${ARG_INPUT} $<TARGET_FILE:assigner>
                      COMMAND_EXPAND_LISTS
                      VERBATIM)
    add_dependencies(run_examples ${example_target}_run)
endfunction()

add_example(arithmetics_example SOURCES arithmetics.cpp INPUT arithmetics.inp)
add_example(poseidon_example SOURCES poseidon.cpp INPUT poseidon.inp)
add_example(merkle_tree_poseidon_example SOURCES merkle_tree_poseidon.cpp INPUT merkle_tree_poseidon.inp)
add_example(merkle_tree_sha2_256_example SOURCES merkle_tree_sha2_256.cpp INPUT merkle_tree_sha2_256.inp)
add_example(sha256_example SOURCES sha2_256.cpp INPUT sha2_256.inp)
add_example(balance_example SOURCES balance.cpp INPUT balance.inp)
add_example(memory_example SOURCES memory.cpp INPUT memory.inp)
add_example(polynomial_example SOURCES polynomial.cpp INPUT polynomial.inp)
add_example(pallas_curve_addition_example
           SOURCES pallas_curve_examples/pallas_curve_add.cpp
           INPUT pallas_curve_examples/pallas_curve_add.inp)
#add_example(pallas_curve_multiplication_example
#            SOURCES pallas_curve_examples/pallas_curve_mul.cpp
#            INPUT pallas_curve_examples/pallas_curve_mul.inp)
#add_example(pallas_curve_mul_add_example
#            SOURCES pallas_curve_examples/pallas_curve_mul_add.cpp
#            INPUT pallas_curve_examples/pallas_curve_mul_add.inp)
add_example(ed25519_curve_add SOURCES ed25519_curve_add.cpp INPUT ed25519_curve_add.inp)
add_example(strlen_example SOURCES strlen.cpp INPUT strlen.inp)
