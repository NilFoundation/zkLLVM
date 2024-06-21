#---------------------------------------------------------------------------#
# Copyright (c) 2018-2022 Mikhail Komarov <nemo@nil.foundation>
# Copyright (c) 2020-2022 Nikita Kaskov <nbering@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.5)

cm_project(recursive_gen WORKSPACE_NAME ${CMAKE_WORKSPACE_NAME} LANGUAGES ASM C CXX)

if (CPACK_PACKAGE_VERSION)
    add_compile_definitions(${UPPER_CURRENT_PROJECT_NAME}_VERSION=${CPACK_PACKAGE_VERSION})
endif()

#Hash is hard-coded poseidon

# get header files; only needed by CMake generators,
# expr.g., for creating proper Xcode projects
set(${CURRENT_PROJECT_NAME}_HEADERS)

# list cpp files excluding platform-dependent files
list(APPEND ${CURRENT_PROJECT_NAME}_SOURCES
     src/main.cpp)

add_executable(${CURRENT_PROJECT_NAME}
               ${${CURRENT_PROJECT_NAME}_HEADERS}
               ${${CURRENT_PROJECT_NAME}_SOURCES})

set_target_properties(${CURRENT_PROJECT_NAME} PROPERTIES
                      LINKER_LANGUAGE CXX
                      EXPORT_NAME ${CURRENT_PROJECT_NAME}
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED TRUE)

target_link_libraries(${CURRENT_PROJECT_NAME}
                      crypto3::all
                      ${Boost_LIBRARIES})

target_include_directories(${CURRENT_PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/circifier/llvm/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/circifier/utils/bazel/llvm-project-overlay/llvm/include
                           ${CMAKE_BINARY_DIR}/libs/circifier/llvm/include

                           ${Boost_INCLUDE_DIRS})

if(APPLE OR NOT ${CMAKE_TARGET_ARCHITECTURE} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR})
    set_target_properties(${CURRENT_PROJECT_NAME} PROPERTIES
                          XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${APPLE_CODE_SIGN_IDENTITY}"
                          XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM}")
elseif(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        set_target_properties(${CURRENT_PROJECT_NAME} PROPERTIES
                              COMPILE_FLAGS "-s USE_BOOST_HEADERS=1 -s EXPORTED_FUNCTIONS=_proof_gen,_main -s EXPORTED_RUNTIME_METHODS=ccall,cwrap"
                              LINK_FLAGS "-s USE_BOOST_HEADERS=1 -s EXPORTED_FUNCTIONS=_proof_gen,_main -s EXPORTED_RUNTIME_METHODS=ccall,cwrap"
                              LINK_DIRECTORIES "${CMAKE_BINARY_DIR}/libs/boost/src/boost/stage/lib")
    endif()

    add_dependencies(${CURRENT_PROJECT_NAME} boost)
endif()
