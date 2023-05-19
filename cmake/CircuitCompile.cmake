#---------------------------------------------------------------------------#
# Copyright (c) 2018-2022 Mikhail Komarov <nemo@nil.foundation>
# Copyright (c) 2020-2022 Nikita Kaskov <nbering@nil.foundation>
# Copyright (c) 2022 Mikhail Aksenov <maksenov@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

function(add_circuit name)
    set(prefix ARG)
    set(noValues "")
    set(singleValues)
    set(multiValues SOURCES INCLUDE_DIRECTORIES LINK_LIBRARIES)
    cmake_parse_arguments(${prefix}
                          "${noValues}"
                          "${singleValues}"
                          "${multiValues}"
                          ${ARGN})

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "SOURCES for ${name} circuit was not defined")
    endif()

    foreach(source ${ARG_SOURCES})
        if(NOT IS_ABSOLUTE ${include_dir})
            list(APPEND CIRCUIT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
        else()
            list(APPEND CIRCUIT_SOURCES "${source}")
        endif()
    endforeach()
    list(REMOVE_DUPLICATES CIRCUIT_SOURCES)

    foreach(ITR ${CIRCUIT_SOURCES})
        if(NOT EXISTS ${ITR})
            message(SEND_ERROR "Cannot find circuit source file: ${source}")
        endif()
    endforeach()

    set(INCLUDE_DIRS_LIST "")
    # Collect include directories from dependencies first
    foreach(lib ${ARG_LINK_LIBRARIES})
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
    if (ZKLLVM_DEV_ENVIRONMENT)
        list(APPEND INCLUDE_DIRS_LIST -I${CMAKE_SOURCE_DIR}/libs/stdlib/libcpp -I${CMAKE_SOURCE_DIR}/libs/stdlib/libc/include)
    endif()
    list(REMOVE_DUPLICATES INCLUDE_DIRS_LIST)

    if(CIRCUIT_ASSEMBLY_OUTPUT)
        set(binary_name ${name}.ll)
        set(format_option -S)
    else()
        set(binary_name ${name}.bc)
        set(format_option -c)
    endif()

    if (ZKLLVM_DEV_ENVIRONMENT)
        set(CLANG $<TARGET_FILE:clang>)
    else()
        set(CLANG clang)
    endif()

    add_custom_target(${name}
                      COMMAND ${CLANG} -target assigner -Xclang -no-opaque-pointers -Xclang -fpreserve-vec3-type -std=c++20
                      -D__ZKLLVM__ ${INCLUDE_DIRS_LIST} -emit-llvm -O1 ${format_option} -o ${binary_name} ${CIRCUIT_SOURCES}

                      VERBATIM COMMAND_EXPAND_LISTS

                      SOURCES ${CIRCUIT_SOURCES})
    set_target_properties(${name} PROPERTIES OUTPUT_NAME ${binary_name})
endfunction(add_circuit)
