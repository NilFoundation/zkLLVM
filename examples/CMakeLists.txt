add_circuit(arithmetics_example SOURCE arithmetics.cpp)
add_circuit(poseidon_example SOURCE poseidon.cpp INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../libs/crypto3)
add_circuit(sha512_example SOURCE sha512.cpp INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../libs/crypto3)
