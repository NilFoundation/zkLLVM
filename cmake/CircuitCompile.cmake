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
    set(singleValues SOURCE)
    set(multiValues INCLUDE_DIRECTORIES)
    cmake_parse_arguments(${prefix}
                          "${noValues}"
                          "${singleValues}"
                          "${multiValues}"
                          ${ARGN})

    if (NOT ARG_SOURCE)
        message(FATAL_ERROR "SOURCE for ${name} circuit was not defined")
    endif()

    set(passed_source ${ARG_SOURCE})
    if (NOT IS_ABSOLUTE ${ARG_SOURCE})
        set(ARG_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_SOURCE}")
    endif()
    if (NOT EXISTS ${ARG_SOURCE})
        message(SEND_ERROR "Cannot find circuit source file: ${passed_source}")
    endif()

    set(INCLUDE_DIRS_STRING "")
    foreach(include_dir ${ARG_INCLUDE_DIRECTORIES})
        if (NOT IS_ABSOLUTE ${include_dir})
            set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/${include_dir}")
        endif()
        string(APPEND INCLUDE_DIRS_STRING "-I${include_dir} ")
    endforeach()

    set(binary_name ${name}.ll)
    add_custom_command(OUTPUT ${binary_name}
                       COMMAND $<TARGET_FILE:clang> ${INCLUDE_DIRS_STRING} -emit-llvm -S -O1 -o ${binary_name} ${ARG_SOURCE}
                       DEPENDS ${ARG_SOURCE}
                       COMMENT "Compiling ${name} circuit"
                       VERBATIM)

    add_custom_target(${name} DEPENDS ${binary_name})
endfunction(add_circuit)
