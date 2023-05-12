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
cmake_policy(SET CMP0063 NEW)

set(Boost_USE_STATIC_LIBS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake"
     "${CMAKE_CURRENT_LIST_DIR}/cmake/packages"
     "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/share/modules/cmake")

include(CMConfig)
include(CMSetupVersion)

cm_workspace(zkllvm)

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
option(CIRCUIT_ASSEMBLY_OUTPUT "Emit circuit IR as assembly file" FALSE)

set(BUILD_WITH_TARGET_ARCHITECTURE "" CACHE STRING "Target build architecture")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/docs" CACHE STRING "Specify doxygen output directory")
set(ZKLLVM_DEV_ENVIRONMENT TRUE)

include(TargetArchitecture)
include(CircuitCompile)

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

add_subdirectories("${CMAKE_CURRENT_LIST_DIR}/libs/crypto3")
add_subdirectories("${CMAKE_CURRENT_LIST_DIR}/libs/crypto3/marshalling")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/blueprint")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/assigner")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/transpiler")

add_subdirectories("${CMAKE_CURRENT_LIST_DIR}/bin")

set(LLVM_ENABLE_PROJECTS "clang" CACHE STRING "Additional llvm projects")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/circifier/llvm")

add_subdirectory(examples)

install(TARGETS assigner RUNTIME DESTINATION bin)
install(FILES cmake/CircuitCompile.cmake DESTINATION share/zkllvm)

find_program(GIT_EXE git)

if(NOT ZKLLVM_VERSION)
	execute_process(COMMAND ${GIT_EXE} describe --tags HEAD
	    OUTPUT_VARIABLE DESCRIBE_OUTBUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

	if (NOT ("${DESCRIBE_OUTBUT}" STREQUAL "${ZKLLVM_VERSION}"))
		set(ZKLLVM_VERSION "${DESCRIBE_OUTBUT}")
    endif()

	if (NOT ZKLLVM_VERSION)
        message (FATAL_ERROR
            "Unable to retrive version from git or ${VERSION_FILE} file.")
    endif()
endif()

# Remove preceding 'v' from tag
string(REGEX REPLACE "^v" "" ZKLLVM_VERSION ${ZKLLVM_VERSION})
# Split numbers
string(REPLACE "-" "." ZKLLVM_VERSION_LIST ${ZKLLVM_VERSION})
string(REPLACE "." ";" ZKLLVM_VERSION_LIST ${ZKLLVM_VERSION_LIST})

LIST(LENGTH ZKLLVM_VERSION_LIST VERSION_LIST_LENGHT)

LIST(GET ZKLLVM_VERSION_LIST 0 ZKLLVM_VERSION_MAJOR)
LIST(GET ZKLLVM_VERSION_LIST 1 ZKLLVM_VERSION_MINOR)
LIST(GET ZKLLVM_VERSION_LIST 2 ZKLLVM_VERSION_PATCH)
if (VERSION_LIST_LENGHT GREATER 3)
    LIST(GET ZKLLVM_VERSION_LIST 3 ZKLLVM_VERSION_INCREMENT)
endif()


set(CPACK_GENERATOR DEB)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "=nil; Foundation")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "zkllvm-stdlib, libstdc++-9-dev")
set(CPACK_PACKAGE_VERSION "${ZKLLVM_VERSION_MAJOR}.${ZKLLVM_VERSION_MINOR}.${ZKLLVM_VERSION_PATCH}")
if (ZKLLVM_VERSION_INCREMENT)
    STRING(APPEND CPACK_PACKAGE_VERSION "-${ZKLLVM_VERSION_INCREMENT}")
endif()
message(STATUS "CPACK_PACKAGE_VERSION=${CPACK_PACKAGE_VERSION}")

include(CPack)

# Package for stdlib
add_custom_target(stdlib_package
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/libs/stdlib/packaging/make_deb.sh ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(rslang
    COMMAND cargo b --release -p cargo
    COMMAND ./x build --stage 1 --config src/bootstrap/defaults/config.zkllvm.toml
    DEPENDS llvm-libraries llvm-config llvm-link FileCheck
    COMMENT "Building rslang"
    USES_TERMINAL
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/libs/rslang")
