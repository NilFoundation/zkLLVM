#---------------------------------------------------------------------------#
# Copyright (c) 2018-2023 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

cm_project(zkllvm_tests WORKSPACE_NAME ${CMAKE_WORKSPACE_NAME} LANGUAGES ASM C CXX)

include(CMTest)

add_custom_target(all_tests_compile_as_cpp_code)
add_custom_target(all_tests_compile_as_circuits)
add_custom_target(all_tests_compilation)
add_custom_target(all_tests_run_expected_res_calculation)
add_custom_target(all_tests_assign_circuits)
add_custom_target(all_tests_run)

if(NOT Boost_UNIT_TEST_FRAMEWORK_FOUND)
    cm_find_package(Boost REQUIRED COMPONENTS unit_test_framework filesystem)
endif()

cm_test_link_libraries(
                       crypto3::zk
                       crypto3::assigner
                       ${Boost_LIBRARIES})


macro(define_zkllvm_test test curve_type provers_max_num)
    string(REPLACE "/" "_" test_name ${test})
    string(REPLACE "/" "_" full_test_name ${test}_calculate_expected_res)

    cm_test(NAME ${full_test_name} SOURCES ${test}.cpp)


    get_filename_component(PARENT_PATH "${test}" DIRECTORY)

    set(inplisdir "${INPUTS_DIR}/${PARENT_PATH}")
    file(GLOB inplist "${inplisdir}/*")
    foreach(full_inp_file ${inplist})
        get_filename_component(curr_inp_name ${full_inp_file} NAME_WE)
        get_filename_component(curr_inp_file ${full_inp_file} NAME)
        run_expected_res_calculation(${full_test_name} ${PARENT_PATH} ${curr_inp_name})
        add_dependencies(all_tests_run_expected_res_calculation ${full_test_name}_run_${curr_inp_name})
        add_test_circuit_assign(${test_name}_${curr_inp_name} SOURCES ${test}.cpp INPUT ${PARENT_PATH}/${curr_inp_file} CURVE_TYPE ${curve_type} MAX_NUM_PROVERS ${provers_max_num})
    endforeach()

    target_include_directories(${full_test_name} PRIVATE
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                               "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>"

                               ${Boost_INCLUDE_DIRS})

    set_target_properties(${full_test_name} PROPERTIES CXX_STANDARD 17)

    # Ignore "circuit" attribute for native compilation
    target_compile_options(${full_test_name} PRIVATE "-Wno-attributes")

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${full_test_name} PRIVATE "-fconstexpr-steps=2147483647")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${full_test_name} PRIVATE "-fconstexpr-ops-limit=4294967295")
    endif()

    get_target_property(target_type Boost::unit_test_framework TYPE)
    if(target_type STREQUAL "SHARED_LIB")
        target_compile_definitions(${full_test_name} PRIVATE BOOST_TEST_DYN_LINK)
    elseif(target_type STREQUAL "STATIC_LIB")

    endif()
    add_dependencies(all_tests_compile_as_cpp_code ${full_test_name})
    add_dependencies(all_tests_compile_as_circuits ${test_name})
endmacro()


function(add_test_circuit test)
    string(REPLACE "/" "_" example_target ${test})

    add_circuit(${example_target}
                SOURCES ${test}.cpp

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

endfunction()

function(add_test_circuit_assign example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues INPUT CURVE_TYPE MAX_NUM_PROVERS)
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

    set(binary_name ${example_target}.ll)

    assign_ir(${example_target} ${binary_name} ${ARG_INPUT} ${ARG_CURVE_TYPE} ${ARG_MAX_NUM_PROVERS})
    add_dependencies(all_tests_assign_circuits ${example_target}_assign)
endfunction()

function(add_zkllvm_unit_test TEST_NAME)
    file(APPEND ../../build/tests/cpp/tests_list_pallas.txt ${TEST_NAME}\n)
    define_zkllvm_test(${TEST_NAME} pallas 0)
    add_test_circuit(${TEST_NAME})
endfunction()

function(add_zkllvm_unit_test_pallas_multiprover TEST_NAME MAX_NUM_PROVERS)
    file(APPEND ../../build/tests/cpp/tests_list_pallas.txt ${TEST_NAME}\n)
    define_zkllvm_test(${TEST_NAME} pallas ${MAX_NUM_PROVERS})
    add_test_circuit(${TEST_NAME})
endfunction()

function(add_zkllvm_unit_test_bls12381 TEST_NAME)
    file(APPEND ../../build/tests/cpp/tests_list_bls12381.txt ${TEST_NAME}\n)
    define_zkllvm_test(${TEST_NAME} bls12381 0)
    add_test_circuit(${TEST_NAME})
endfunction()

file(REMOVE ../../build/tests/cpp/tests_list_pallas.txt)
file(REMOVE ../../build/tests/cpp/tests_list_bls12381.txt)

file(REMOVE ../../build/tests/cpp/circuits_outputs_list.txt)
file(REMOVE ../../build/tests/cpp/expected_results_list.txt)


# curve tests
add_zkllvm_unit_test("algebra/curves/25519_arithmetics/compare")
add_zkllvm_unit_test("algebra/curves/25519_arithmetics/add")
add_zkllvm_unit_test("algebra/curves/25519_arithmetics_2/mul")

if(APPLE)
add_zkllvm_unit_test("algebra/curves/constructors/zero_pallas")
add_zkllvm_unit_test("algebra/curves/constructors/one_pallas")
add_zkllvm_unit_test("algebra/curves/constructors/zero_ed25519")
add_zkllvm_unit_test("algebra/curves/constructors/one_ed25519")
endif()

add_zkllvm_unit_test("algebra/curves/pallas_arithmetics/compare")
# add_zkllvm_unit_test("algebra/curves/pallas_arithmetics/add") # fails when return value is 0,1 due to blueprint bug, temporarily disabled
# add_zkllvm_unit_test("algebra/curves/pallas_arithmetics_2/mul") # fails because blueprint component requires prepared scalar, tempotatily disabled


# field tests
add_zkllvm_unit_test("algebra/fields/25519_arithmetics/add")
# add_zkllvm_unit_test("algebra/fields/25519_arithmetics/mul")  # fails on neg1_neg1 due to bug in blueprint, temporarily disabled
add_zkllvm_unit_test("algebra/fields/25519_arithmetics/sub")

add_zkllvm_unit_test("algebra/fields/comparison/greater")
add_zkllvm_unit_test("algebra/fields/comparison/grequal")
add_zkllvm_unit_test("algebra/fields/comparison/lequal")
add_zkllvm_unit_test("algebra/fields/comparison/less")
add_zkllvm_unit_test("algebra/fields/comparison/equal")
add_zkllvm_unit_test("algebra/fields/comparison/not_equal")

add_zkllvm_unit_test("algebra/fields/pallas_arithmetics/add")
add_zkllvm_unit_test("algebra/fields/pallas_arithmetics/loop")
add_zkllvm_unit_test("algebra/fields/pallas_arithmetics/mul")
add_zkllvm_unit_test("algebra/fields/pallas_arithmetics/sub")

# add_zkllvm_unit_test("algebra/fields/private_input/private_input_add")
add_zkllvm_unit_test("algebra/fields/conversion/itogf_pallas")

# int tests
add_zkllvm_unit_test("algebra/integers/division/udiv")
add_zkllvm_unit_test("algebra/integers/division/urem")

add_zkllvm_unit_test("algebra/integers/shift/left")
add_zkllvm_unit_test("algebra/integers/shift/right")

#bool tests
add_zkllvm_unit_test("algebra/bool/assigner_exit_check")

# bit de/composition tests
add_zkllvm_unit_test("algebra/de_composition/decomposition/msb")
add_zkllvm_unit_test("algebra/de_composition/decomposition/lsb")

#multi provers tests
add_zkllvm_unit_test_pallas_multiprover("multi_provers/shift_add" 2)
add_zkllvm_unit_test_pallas_multiprover("multi_provers/if" 2)

#customer's tests
add_zkllvm_unit_test("custom_cases/getelementptr_as_constant/getelementptr_as_constant")

#libc functions
add_zkllvm_unit_test("libc/memset/memset")
#add_zkllvm_unit_test("libc/strlen/strlen")
#add_zkllvm_unit_test("libc/strcmp/strcmp")
#add_zkllvm_unit_test("libc/strncmp/strncmp")
#add_zkllvm_unit_test("libc/strcpy/strcpy")
#add_zkllvm_unit_test("libc/strncpy/strncpy")

# hashes
add_zkllvm_unit_test("hashes/sha256/sha2_256")
add_zkllvm_unit_test("hashes/poseidon/poseidon")

# recursive prover tests
add_zkllvm_unit_test("algebra/recursive_prover/fri_lin_inter/fri_lin_inter")
add_zkllvm_unit_test("algebra/recursive_prover/fri_cosets/fri_cosets")
add_zkllvm_unit_test("algebra/recursive_prover/gate_arg_verifier/1gate_1constr/gate_arg_verifier")
add_zkllvm_unit_test("algebra/recursive_prover/permutation_arg_verifier/input_length_4/test")
add_zkllvm_unit_test("algebra/recursive_prover/fri_array_swap_length_6/test")
add_zkllvm_unit_test("algebra/recursive_prover/lookup_arg_verifier/test1/test")
add_zkllvm_unit_test("algebra/recursive_prover/lookup_arg_verifier/test2/test")

# bls tests
add_zkllvm_unit_test_bls12381("algebra/fields/bls12381_fp12_arithmetics/mul")
add_zkllvm_unit_test_bls12381("bls_signature/pairing/pairing")

add_zkllvm_unit_test("nested_if/first")
add_zkllvm_unit_test("nested_if/second")

add_dependencies(all_tests_compilation assigner)
add_dependencies(all_tests_compilation all_tests_compile_as_circuits)
add_dependencies(all_tests_compilation all_tests_compile_as_cpp_code)
add_dependencies(all_tests_run all_tests_run_expected_res_calculation)
add_dependencies(all_tests_run all_tests_assign_circuits)
