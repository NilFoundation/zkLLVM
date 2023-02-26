add_custom_target(circuit_examples)

macro(add_example example_target)
    add_circuit(${ARGV})
    add_dependencies(circuit_examples "${example_target}")
endmacro()

add_example(useless_test_example_1_gates SOURCE useless_test_example_1_gates.cpp)
add_example(useless_test_example_2_gates SOURCE useless_test_example_2_gates.cpp)
add_example(useless_test_example_3_gates SOURCE useless_test_example_3_gates.cpp)
add_example(useless_test_example_4_gates SOURCE useless_test_example_4_gates.cpp)
add_example(useless_test_example_5_gates SOURCE useless_test_example_5_gates.cpp)
add_example(useless_test_example_6_gates SOURCE useless_test_example_6_gates.cpp)
add_example(useless_test_example_7_gates SOURCE useless_test_example_7_gates.cpp)