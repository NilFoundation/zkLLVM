#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#
#
# Check if SSE instructions are available on the machine where
# the project is compiled.

macro(check_sse)
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        exec_program(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)

        string(REGEX REPLACE "^.*(sse2).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "sse2" "${SSE_THERE}" SSE2_TRUE)
        if(SSE2_TRUE)
            set(SSE2_FOUND true CACHE BOOL "SSE2 available on host")
        else(SSE2_TRUE)
            set(SSE2_FOUND false CACHE BOOL "SSE2 available on host")
        endif(SSE2_TRUE)

        # /proc/cpuinfo apparently omits sse3 :(
        string(REGEX REPLACE "^.*[^s](sse3).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "sse3" "${SSE_THERE}" SSE3_TRUE)
        if(NOT SSE3_TRUE)
            string(REGEX REPLACE "^.*(T2300).*$" "\\1" SSE_THERE ${CPUINFO})
            string(COMPARE EQUAL "T2300" "${SSE_THERE}" SSE3_TRUE)
        endif(NOT SSE3_TRUE)

        string(REGEX REPLACE "^.*(ssse3).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "ssse3" "${SSE_THERE}" SSSE3_TRUE)
        if(SSE3_TRUE OR SSSE3_TRUE)
            set(SSE3_FOUND true CACHE BOOL "SSE3 available on host")
        else(SSE3_TRUE OR SSSE3_TRUE)
            set(SSE3_FOUND false CACHE BOOL "SSE3 available on host")
        endif(SSE3_TRUE OR SSSE3_TRUE)
        if(SSSE3_TRUE)
            set(SSSE3_FOUND true CACHE BOOL "SSSE3 available on host")
        else(SSSE3_TRUE)
            set(SSSE3_FOUND false CACHE BOOL "SSSE3 available on host")
        endif(SSSE3_TRUE)

        string(REGEX REPLACE "^.*(sse4_1).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "sse4_1" "${SSE_THERE}" SSE41_TRUE)
        if(SSE41_TRUE)
            set(SSE4_1_FOUND true CACHE BOOL "SSE4.1 available on host")
        else(SSE41_TRUE)
            set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
        endif(SSE41_TRUE)

        string(REGEX REPLACE "^.*(sse4_2).*$" "\\1" SSE_THERE ${CPUINFO})
        string(COMPARE EQUAL "sse4_2" "${SSE_THERE}" SSE42_TRUE)
        if(SSE42_TRUE)
            set(SSE4_2_FOUND true CACHE BOOL "SSE4.2 available on host")
        else(SSE42_TRUE)
            set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")
        endif(SSE42_TRUE)

    elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        exec_program("/usr/sbin/sysctl -n machdep.cpu.features" OUTPUT_VARIABLE
                     CPUINFO)

        string(REGEX REPLACE "^.*[^S](SSE2).*$" "\\1" SSE_THERE "${CPUINFO}")
        string(COMPARE EQUAL "SSE2" "${SSE_THERE}" SSE2_TRUE)
        if(SSE2_TRUE)
            set(SSE2_FOUND true CACHE BOOL "SSE2 available on host")
        else(SSE2_TRUE)
            set(SSE2_FOUND false CACHE BOOL "SSE2 available on host")
        endif(SSE2_TRUE)

        string(REGEX REPLACE "^.*[^S](SSE3).*$" "\\1" SSE_THERE "${CPUINFO}")
        string(COMPARE EQUAL "SSE3" "${SSE_THERE}" SSE3_TRUE)
        if(SSE3_TRUE)
            set(SSE3_FOUND true CACHE BOOL "SSE3 available on host")
        else(SSE3_TRUE)
            set(SSE3_FOUND false CACHE BOOL "SSE3 available on host")
        endif(SSE3_TRUE)

        string(REGEX REPLACE "^.*(SSSE3).*$" "\\1" SSE_THERE "${CPUINFO}")
        string(COMPARE EQUAL "SSSE3" "${SSE_THERE}" SSSE3_TRUE)
        if(SSSE3_TRUE)
            set(SSSE3_FOUND true CACHE BOOL "SSSE3 available on host")
        else(SSSE3_TRUE)
            set(SSSE3_FOUND false CACHE BOOL "SSSE3 available on host")
        endif(SSSE3_TRUE)

        string(REGEX REPLACE "^.*(SSE4.1).*$" "\\1" SSE_THERE "${CPUINFO}")
        string(COMPARE EQUAL "SSE4.1" "${SSE_THERE}" SSE41_TRUE)
        if(SSE41_TRUE)
            set(SSE4_1_FOUND true CACHE BOOL "SSE4.1 available on host")
        else(SSE41_TRUE)
            set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
        endif(SSE41_TRUE)

        string(REGEX REPLACE "^.*(SSE4.2).*$" "\\1" SSE_THERE "${CPUINFO}")
        string(COMPARE EQUAL "SSE4.2" "${SSE_THERE}" SSE42_TRUE)
        if(SSE42_TRUE)
            set(SSE4_2_FOUND true CACHE BOOL "SSE4.2 available on host")
        else(SSE42_TRUE)
            set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")
        endif(SSE42_TRUE)

    elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
        # TODO
        set(SSE2_FOUND true CACHE BOOL "SSE2 available on host")
        set(SSE3_FOUND false CACHE BOOL "SSE3 available on host")
        set(SSSE3_FOUND false CACHE BOOL "SSSE3 available on host")
        set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
        set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")
    else(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(SSE2_FOUND true CACHE BOOL "SSE2 available on host")
        set(SSE3_FOUND false CACHE BOOL "SSE3 available on host")
        set(SSSE3_FOUND false CACHE BOOL "SSSE3 available on host")
        set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
        set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host")
    endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

    if(CMAKE_COMPILER_IS_GNUCXX)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
        if(GCC_VERSION VERSION_LESS 4.2)
            set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host" FORCE)
            set(SSE4_2_FOUND false CACHE BOOL "SSE4.2 available on host" FORCE)
        endif()
    endif(CMAKE_COMPILER_IS_GNUCXX)

    if(NOT SSE2_FOUND)
        message(STATUS "Could not find support for SSE2 on this machine.")
    endif(NOT SSE2_FOUND)
    if(NOT SSE3_FOUND)
        message(STATUS "Could not find support for SSE3 on this machine.")
    endif(NOT SSE3_FOUND)
    if(NOT SSSE3_FOUND)
        message(STATUS "Could not find support for SSSE3 on this machine.")
    endif(NOT SSSE3_FOUND)
    if(NOT SSE4_1_FOUND)
        message(STATUS "Could not find support for SSE4.1 on this machine.")
    endif(NOT SSE4_1_FOUND)
    if(NOT SSE4_2_FOUND)
        message(STATUS "Could not find support for SSE4.2 on this machine.")
    endif(NOT SSE4_2_FOUND)

    mark_as_advanced(SSE2_FOUND SSE3_FOUND SSSE3_FOUND SSE4_1_FOUND SSE4_2_FOUND)

endmacro(check_sse)
