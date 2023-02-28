add_custom_target(circuit_examples)

macro(add_example example_target)
    add_circuit(${ARGV}

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
endmacro()

add_example(arithmetics_example

            SOURCES arithmetics.cpp)

add_example(poseidon_example SOURCES poseidon.cpp

            INCLUDE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/../libs/crypto3)

add_example(sha256_example
            SOURCES sha2-256.cpp

            INCLUDE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/../libs/crypto3)

add_example(polynomial_example
            SOURCES polynomial.cpp)

set_target_properties(polynomial_example PROPERTIES
                      CXX_STANDARD 20
                      CXX_STANDARD_REQUIRED TRUE)

add_example(pallas_curve_addition_example
            SOURCES pallas_curve_examples/pallas_curve_add.cpp)

add_example(pallas_curve_multiplication_example
            SOURCES pallas_curve_examples/pallas_curve_mul.cpp)

add_example(pallas_curve_mul_add_example
            SOURCES pallas_curve_examples/pallas_curve_mul_add.cpp)

add_example(ed25519_curve_add
            SOURCES ed25519_curve_add.cpp)

add_example(balance_example
            SOURCES balance.cpp)
