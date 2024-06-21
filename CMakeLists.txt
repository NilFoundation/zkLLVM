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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake"
     "${CMAKE_CURRENT_LIST_DIR}/cmake/packages"
     "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/share/modules/cmake")

include(CMConfig)
include(CMSetupVersion)

cm_workspace(zkllvm)
cm_setup_version(VERSION 0.3.0 PREFIX ${CMAKE_WORKSPACE_NAME})

option(BUILD_SHARED_LIBS "Build shared library" TRUE)
option(BUILD_WITH_BOOST_STATIC_LIBS "Build using Boost static libraries" TRUE)
option(CMAKE_ENABLE_TESTS "Build unit tests" FALSE)
option(BUILD_DOCS "Build with configuring Doxygen documentation compiler" TRUE)
option(CIRCUIT_BINARY_OUTPUT "Emit circuit IR as binary file" FALSE)
option(RSLANG_BUILD_EXTENDED "Build Rust tools (see build.extended)" FALSE)

set(BUILD_WITH_TARGET_ARCHITECTURE "" CACHE STRING "Target build architecture")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/docs" CACHE STRING "Specify doxygen output directory")
set(ZKLLVM_DEV_ENVIRONMENT TRUE)
set(RSLANG_BUILD_STAGE "2" CACHE STRING "Stage to build (see --stage)")
set(RSLANG_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/libs/rslang/config.toml CACHE STRING "Path to Rust config file")
set(RSLANG_BUILD_TOOLS "<none>" CACHE STRING "Rust tools to build (see build.tools)")

include(TargetArchitecture)
include(CircuitCompile)

find_package(Python3 3.6 REQUIRED COMPONENTS Interpreter)

find_package(crypto3 REQUIRED)

find_package(Boost COMPONENTS REQUIRED filesystem log log_setup program_options thread system random)
if(CMAKE_ENABLE_TESTS)
    find_package(Boost REQUIRED COMPONENTS unit_test_framework timer)
endif()

if(BUILD_WITH_BOOST_STATIC_LIBS)
    set(Boost_USE_STATIC_LIBS TRUE)
else()
    set(Boost_USE_STATIC_LIBS FALSE)
endif()

if(BUILD_WITH_TARGET_ARCHITECTURE)
    set(CMAKE_TARGET_ARCHITECTURE ${BUILD_WITH_TARGET_ARCHITECTURE})
else()
    target_architecture(CMAKE_TARGET_ARCHITECTURE)
endif()

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/blueprint")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/assigner")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/transpiler")

set(LLVM_ENABLE_PROJECTS "clang" CACHE STRING "Additional llvm projects")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/circifier/llvm")

include(CMTest)
cm_add_test_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(libs/stdlib)

install(FILES cmake/CircuitCompile.cmake DESTINATION share/zkllvm)

set(ZKLLVM_VERSION "1.2.3")
if(NOT ZKLLVM_VERSION)
    find_package(Git REQUIRED)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags HEAD
                    OUTPUT_VARIABLE DESCRIBE_OUTBUT
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

    if(NOT ("${DESCRIBE_OUTBUT}" STREQUAL "${ZKLLVM_VERSION}"))
        set(ZKLLVM_VERSION "${DESCRIBE_OUTBUT}")
    endif()

    if(NOT ZKLLVM_VERSION)
        message(FATAL_ERROR
                "Unable to retrive version from git or ${VERSION_FILE} file.")
    endif()
endif()

# Remove preceding 'v' from tag
string(REGEX REPLACE "^v" "" ZKLLVM_VERSION ${ZKLLVM_VERSION})
# Split numbers
string(REPLACE "-" "." ZKLLVM_VERSION_LIST ${ZKLLVM_VERSION})
string(REPLACE "." ";" ZKLLVM_VERSION_LIST ${ZKLLVM_VERSION_LIST})

list(LENGTH ZKLLVM_VERSION_LIST VERSION_LIST_LENGHT)

list(GET ZKLLVM_VERSION_LIST 0 ZKLLVM_VERSION_MAJOR)
list(GET ZKLLVM_VERSION_LIST 1 ZKLLVM_VERSION_MINOR)
list(GET ZKLLVM_VERSION_LIST 2 ZKLLVM_VERSION_PATCH)
if(VERSION_LIST_LENGHT GREATER 3)
    list(GET ZKLLVM_VERSION_LIST 3 ZKLLVM_VERSION_INCREMENT)
endif()


set(CPACK_GENERATOR DEB)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "=nil; Foundation")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "build-essential")
set(CPACK_PACKAGE_VERSION "${ZKLLVM_VERSION_MAJOR}.${ZKLLVM_VERSION_MINOR}.${ZKLLVM_VERSION_PATCH}")
if(ZKLLVM_VERSION_INCREMENT)
    string(APPEND CPACK_PACKAGE_VERSION "-${ZKLLVM_VERSION_INCREMENT}")
endif()
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/lib/zkllvm")

configure_file("${CMAKE_CURRENT_LIST_DIR}/cmake/zkllvm.conf.in"
               "${CMAKE_CURRENT_BINARY_DIR}/zkllvm.conf"
               NO_SOURCE_PERMISSIONS
               @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zkllvm.conf" DESTINATION /etc/ld.so.conf.d)

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_LIST_DIR}/cmake/triggers")
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)

message(${CMAKE_INSTALL_FULL_LIBDIR})

install(SCRIPT cmake/create-symlinks.cmake)

include(CPack)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/bin")

message(STATUS "Configuring rslang")

execute_process(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/detect-platform.py
                RESULT_VARIABLE DETECT_PLATFORM_RESULT
                OUTPUT_VARIABLE RUSTC_HOST_TRIPLE
                OUTPUT_STRIP_TRAILING_WHITESPACE)

if(NOT ${DETECT_PLATFORM_RESULT} EQUAL 0)
    message(FATAL_ERROR "Failed to detect host platform")
endif()

list(APPEND RSLANG_BUILD_ARGS --set llvm.download-ci-llvm=false)
list(APPEND RSLANG_BUILD_ARGS --release-channel=nightly)
list(APPEND RSLANG_BUILD_ARGS "--release-description=zkLLVM ${ZKLLVM_VERSION_MAJOR}.${ZKLLVM_VERSION_MINOR}.${ZKLLVM_VERSION_PATCH}")
list(APPEND RSLANG_BUILD_ARGS --target=${RUSTC_HOST_TRIPLE},assigner-unknown-unknown)
list(APPEND RSLANG_BUILD_ARGS --set target.${RUSTC_HOST_TRIPLE}.llvm-config=${CMAKE_CURRENT_BINARY_DIR}/libs/circifier/llvm/bin/llvm-config)
list(APPEND RSLANG_BUILD_ARGS --set target.assigner-unknown-unknown.llvm-config=${CMAKE_CURRENT_BINARY_DIR}/libs/circifier/llvm/bin/llvm-config)
list(APPEND RSLANG_BUILD_ARGS --set target.assigner-unknown-unknown.linker=llvm-link)
if(${BUILD_SHARED_LIBS})
    list(APPEND RSLANG_BUILD_ARGS --enable-llvm-link-shared)
endif()
list(APPEND RSLANG_BUILD_ARGS --set build.build-dir=${CMAKE_CURRENT_BINARY_DIR}/libs/rslang/build)
list(APPEND RSLANG_BUILD_ARGS --set build.build-stage=${RSLANG_BUILD_STAGE})
list(APPEND RSLANG_BUILD_ARGS --set build.cargo=${CMAKE_CURRENT_BINARY_DIR}/libs/rslang/target/release/cargo)
if(${RSLANG_BUILD_EXTENDED})
    list(APPEND RSLANG_BUILD_ARGS --enable-extended)
    if(NOT RSLANG_BUILD_TOOLS STREQUAL "<none>")
        list(APPEND RSLANG_BUILD_ARGS --tools=${RSLANG_BUILD_TOOLS})
    endif()
else()
    list(APPEND RSLANG_BUILD_ARGS --disable-extended)
endif()
if(CMAKE_BUILD_TYPE MATCHES Debug)
    list(APPEND RSLANG_BUILD_ARGS --enable-debug)
endif()
list(APPEND RSLANG_BUILD_ARGS --dist-compression-formats=gz)
# TODO: (aleasims) re-enable docs after fixing all errors:
# Issue: https://github.com/NilFoundation/zkllvm-rslang/issues/74
list(APPEND RSLANG_BUILD_ARGS --disable-docs)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs/rslang)
file(REMOVE ${RSLANG_CONFIG})
execute_process(COMMAND ${CMAKE_CURRENT_LIST_DIR}/libs/rslang/configure ${RSLANG_BUILD_ARGS}
                COMMAND_ERROR_IS_FATAL ANY
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs/rslang)

add_custom_target(cargo
                  COMMAND cargo build --release --target-dir ${CMAKE_CURRENT_BINARY_DIR}/libs/rslang/target
                  COMMENT "Building cargo"
                  USES_TERMINAL
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/libs/rslang/src/tools/cargo)

add_custom_target(rslang
                  COMMAND ./x build --config ${RSLANG_CONFIG}
                  DEPENDS llvm-libraries llvm-config llvm-link FileCheck cargo
                  COMMENT "Building rslang"
                  USES_TERMINAL
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/libs/rslang)

add_custom_target(rslang_dist
                  COMMAND ./x dist --config ${RSLANG_CONFIG}
                  DEPENDS rslang
                  COMMENT "Building rslang distribution package"
                  USES_TERMINAL
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/libs/rslang)
