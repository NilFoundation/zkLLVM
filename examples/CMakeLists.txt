add_circuit(arithmetics_example SOURCE arithmetics.cpp)
add_circuit(poseidon_example SOURCE poseidon.cpp INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../libs/crypto3)
add_circuit(sha256_example SOURCE sha2-256.cpp INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../libs/crypto3)
add_circuit(curve_example SOURCE curves.cpp)

add_custom_target(circuit_examples DEPENDS arithmetics_example poseidon_example sha256_example)
