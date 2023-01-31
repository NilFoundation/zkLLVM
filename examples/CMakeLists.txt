add_custom_target(circuit_examples)

macro(add_example example_target)
    add_circuit(${ARGV})
    add_dependencies(circuit_examples "${example_target}")
endmacro()


add_example(arithmetics_example SOURCE arithmetics.cpp)
add_example(poseidon_example SOURCE poseidon.cpp INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../libs/crypto3)
add_example(sha256_example SOURCE sha2-256.cpp INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../libs/crypto3)
add_example(curve_example SOURCE curves.cpp)
add_example(polynomial_example SOURCE polynomial.cpp)
