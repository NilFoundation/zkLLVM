#---------------------------------------------------------------------------#
# Copyright (c) 2018-2022 Mikhail Komarov <nemo@nil.foundation>
# Copyright (c) 2020-2022 Nikita Kaskov <nbering@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.2)

cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0028 NEW)
cmake_policy(SET CMP0057 NEW)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake"
     "${CMAKE_CURRENT_LIST_DIR}/cmake/packages"
     "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/share/modules/cmake")

include(CMConfig)
include(CMSetupVersion)
include(CircuitCompile)

cm_workspace(crypto3)

macro(cm_find_package NAME)
    if(NOT "${NAME}" MATCHES "^${CMAKE_WORKSPACE_NAME}_.*$" AND NOT "${NAME}" STREQUAL CM)
        find_package(${ARGV})
    else()
        set(${ARGV0}_FOUND ON CACHE BOOL "")
    endif()
endmacro()

cm_setup_version(VERSION 0.3.0 PREFIX ${CMAKE_WORKSPACE_NAME})

option(BUILD_SHARED_LIBS "Build shared library" TRUE)
option(BUILD_WITH_CCACHE "Build with ccache usage" TRUE)
option(BUILD_TESTS "Build unit tests" FALSE)
option(BUILD_DOCS "Build with configuring Doxygen documentation compiler" TRUE)

set(BUILD_WITH_TARGET_ARCHITECTURE "" CACHE STRING "Target build architecture")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/docs" CACHE STRING "Specify doxygen output directory")

include(TargetArchitecture)

if(UNIX AND BUILD_WITH_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)
endif()

if(BUILD_WITH_TARGET_ARCHITECTURE)
    set(CMAKE_TARGET_ARCHITECTURE ${BUILD_WITH_TARGET_ARCHITECTURE})
else()
    target_architecture(CMAKE_TARGET_ARCHITECTURE)
endif()

if(BUILD_TESTS)
    cm_find_package(Boost COMPONENTS REQUIRED unit_test_framework)
endif()

add_subdirectories("${CMAKE_CURRENT_LIST_DIR}/libs/crypto3"
                   SUBMODULE_DEFINITION_HEADER "cm_project\\(")
add_subdirectories("${CMAKE_CURRENT_LIST_DIR}/libs/crypto3/marshalling")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/blueprint")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/assigner")
add_subdirectories("${CMAKE_CURRENT_LIST_DIR}/bin")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/circifier/llvm")
add_subdirectory(examples)
