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
    string(REPLACE "/" "_" test_name ${test})
    string(REPLACE "/" "_" full_test_name ${test}_calculate_expected_res)

    cm_test(NAME ${full_test_name} SOURCES ${test}.cpp)

    get_filename_component(PARENT_PATH "${test}" DIRECTORY)

    set(inplisdir "${INPUTS_DIR}/${PARENT_PATH}")
    message(inplisdir: "${inplisdir}")
    file(GLOB inplist "${inplisdir}/*")
    foreach(full_inp_file ${inplist})
        get_filename_component(curr_inp_name ${full_inp_file} NAME_WE)
        get_filename_component(curr_inp_file ${full_inp_file} NAME)
        run_expected_res_calculation(${full_test_name} ${PARENT_PATH} ${curr_inp_name})
        add_test_circuit_assign(${test_name}_${curr_inp_name} SOURCES ${test}.cpp INPUT ${PARENT_PATH}/${curr_inp_file})
    endforeach()
    message(end inplist)

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
    add_dependencies(circuit_cpp_examples "${example_target}")

endfunction()

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
    message("example_target: ${example_target}")
    message("ARG_SOURCES: ${ARG_SOURCES}")
    message("ARG_INPUT: ${ARG_INPUT}")
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

set(TESTS_NAMES
    "algebra/fields/native_pallas_arithmetics/add"
    "algebra/fields/native_pallas_arithmetics/mul"
    "algebra/fields/native_pallas_arithmetics/sub"
    "algebra/integers/shift/left"
    "algebra/integers/shift/right"
    )

file(REMOVE ../tests_list.txt)

foreach(TEST_NAME ${TESTS_NAMES})
    file(APPEND ../tests_list.txt ${TEST_NAME}\n)
    define_zkllvm_test(${TEST_NAME})
    add_test_circuit(${TEST_NAME})
endforeach()



# foreach(TEST_NAME ${TESTS_NAMES})
#     define_assign_and_run_targets(${TEST_NAME})
# endforeach()
