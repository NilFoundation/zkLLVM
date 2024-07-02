#---------------------------------------------------------------------------#
# Copyright (c) 2024 Alexey Kokoshnikov <alexeikokoshnikov@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.5)

cm_find_package(CM)
include(CMDeploy)

set(WITNESS_COLUMNS 15 CACHE STRING "Number of witness columns")
set(PUBLIC_INPUT_COLUMNS 1 CACHE STRING "Number of public input columns")
set(COMPONENT_CONSTANT_COLUMNS 5 CACHE STRING "Number of component constant columns")
set(LOOKUP_CONSTANT_COLUMNS 30 CACHE STRING "Number of lookup constant columns")
set(COMPONENT_SELECTOR_COLUMNS 50 CACHE STRING "Number of component selector columns")
set(LOOKUP_SELECTOR_COLUMNS 6 CACHE STRING "Number of lookup selector columns")
set(SECURITY_PARAMETER_LAMBDA 9 CACHE STRING "Number of FRI queries")
set(SECURITY_PARAMETER_GRINDING_BITS 0 CACHE STRING "Number of FRI grinding bits")

add_definitions(-DWITNESS_COLUMNS=${WITNESS_COLUMNS})
add_definitions(-DPUBLIC_INPUT_COLUMNS=${PUBLIC_INPUT_COLUMNS})
add_definitions(-DCOMPONENT_CONSTANT_COLUMNS=${COMPONENT_CONSTANT_COLUMNS})
add_definitions(-DLOOKUP_CONSTANT_COLUMNS=${LOOKUP_CONSTANT_COLUMNS})
add_definitions(-DCOMPONENT_SELECTOR_COLUMNS=${COMPONENT_SELECTOR_COLUMNS})
add_definitions(-DLOOKUP_SELECTOR_COLUMNS=${LOOKUP_SELECTOR_COLUMNS})
add_definitions(-DLAMBDA=${SECURITY_PARAMETER_LAMBDA})
add_definitions(-DGRINDING_BITS=${SECURITY_PARAMETER_GRINDING_BITS})

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/assigner")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/recursive_gen")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/transpiler")
