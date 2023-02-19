#---------------------------------------------------------------------------#
# Copyright (c) 2018-2022 Mikhail Komarov <nemo@nil.foundation>
# Copyright (c) 2020-2022 Nikita Kaskov <nbering@nil.foundation>
# Copyright (c) 2022 Mikhail Aksenov <maksenov@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

set(CIRCUIT_DEPENDENCIES
    crypto3::algebra
    crypto3::block
    crypto3::blueprint
    crypto3::codec
    crypto3::math
    crypto3::multiprecision
    crypto3::pkpad
    crypto3::pubkey
    crypto3::random
    crypto3::zk
    crypto3::assigner
    marshalling::core
    marshalling::crypto3_algebra
    marshalling::crypto3_multiprecision
    marshalling::crypto3_zk)

function(add_circuit name)
    set(prefix ARG)
    set(noValues "")
    set(singleValues SOURCE)
    set(multiValues INCLUDE_DIRECTORIES)
    cmake_parse_arguments(${prefix}
                          "${noValues}"
                          "${singleValues}"
                          "${multiValues}"
                          ${ARGN})

    if(NOT ARG_SOURCE)
        message(FATAL_ERROR "SOURCE for ${name} circuit was not defined")
    endif()

    set(passed_source ${ARG_SOURCE})
    if(NOT IS_ABSOLUTE ${ARG_SOURCE})
        set(ARG_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_SOURCE}")
    endif()
    if(NOT EXISTS ${ARG_SOURCE})
        message(SEND_ERROR "Cannot find circuit source file: ${passed_source}")
    endif()

    set(INCLUDE_DIRS_LIST "")
    # Collect include directories from dependencies first
    foreach(lib ${CIRCUIT_DEPENDENCIES})
        get_target_property(lib_include_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
        foreach(dir ${lib_include_dirs})
            list(APPEND INCLUDE_DIRS_LIST "-I${dir}")
        endforeach()
    endforeach()
    # Add passed include directories
    foreach(include_dir ${ARG_INCLUDE_DIRECTORIES})
        if(NOT IS_ABSOLUTE ${include_dir})
            set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/${include_dir}")
        endif()
        list(APPEND INCLUDE_DIRS_LIST "-I${include_dir}")
    endforeach()
    list(REMOVE_DUPLICATES INCLUDE_DIRS_LIST)

    if(CIRCUIT_ASSEMBLY_OUTPUT)
        set(binary_name ${name}.ll)
        set(format_option -S)
    else()
        set(binary_name ${name}.bc)
        set(format_option -c)
    endif()

    add_custom_target(${name} COMMAND_EXPAND_LISTS VERBATIM

                      COMMAND $<TARGET_FILE:clang> -D__ZKLLVM__ ${INCLUDE_DIRS_LIST} -emit-llvm -O1
                      ${format_option} -o ${binary_name} ${ARG_SOURCE}

                      SOURCES ${ARG_SOURCE})
    set_target_properties(${name} PROPERTIES
                          CXX_STANDARD 20
                          CXX_STANDARD_REQUIRED TRUE
                          OUTPUT_NAME ${binary_name})
endfunction(add_circuit)
