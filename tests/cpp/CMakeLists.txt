#---------------------------------------------------------------------------#
# Copyright (c) 2018-2023 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

cm_project(zkllvm_tests WORKSPACE_NAME ${CMAKE_WORKSPACE_NAME} LANGUAGES ASM C CXX)

include(CMTest)

if(NOT Boost_UNIT_TEST_FRAMEWORK_FOUND)
    cm_find_package(Boost REQUIRED COMPONENTS unit_test_framework filesystem)
endif()

cm_test_link_libraries(
                       crypto3::zk
                       ${Boost_LIBRARIES})


macro(define_zkllvm_test test)
    string(REPLACE "/" "_" full_test_name ${test}_calculate_expected_res)

    cm_test(NAME ${full_test_name} SOURCES ${test}.cpp)
    message("test: ${test}")
    message("full_test_name: ${full_test_name}")

    target_include_directories(${full_test_name} PRIVATE
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                               "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>"

                               ${Boost_INCLUDE_DIRS})

    set_target_properties(${full_test_name} PROPERTIES CXX_STANDARD 17)

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
endmacro()

function(add_test_circuit_assign example_target)
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

function(add_test_run_no_circuit example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues INPUT)
    set(multiValues SOURCES)
    cmake_parse_arguments(${prefix}
                        "${noValues}"
                        "${singleValues}"
                        "${multiValues}"
                        ${ARGN})
    add_dependencies(circuit_cpp_examples "${example_target}")

#    assign_ir(${example_target} ${binary_name} ${ARG_INPUT})
    run_test_no_circuit(${example_target} ${ARG_INPUT})
#    add_dependencies(assign_cpp_examples ${example_target}_assign)
endfunction()

set(TESTS_NAMES
    "algebra/fields/pallas_add"
    "algebra/fields/pallas_mul"
    )


add_test_circuit_assign(algebra_fields_pallas_add SOURCES algebra/fields/pallas_add.cpp INPUT algebra/fields/pallas_add0.inp)
add_test_run_no_circuit(algebra_fields_pallas_add_calculate_expected_res SOURCES algebra/fields/pallas_add.cpp INPUT algebra/fields/pallas_add0.inp)

add_test_circuit_assign(algebra_fields_pallas_mul SOURCES algebra/fields/pallas_mul.cpp INPUT algebra/fields/pallas_mul0.inp)
add_test_run_no_circuit(algebra_fields_pallas_mul_calculate_expected_res SOURCES algebra/fields/pallas_mul.cpp INPUT algebra/fields/pallas_mul0.inp)

foreach(TEST_NAME ${TESTS_NAMES})
    define_zkllvm_test(${TEST_NAME})
    message("test name: " ${TEST_NAME})
    # add_test_circuit_assign(${TEST_NAME})
endforeach()
