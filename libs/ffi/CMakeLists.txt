cmake_minimum_required(VERSION 2.8.12)

cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0028 NEW)

cm_project(ffi WORKSPACE_NAME ${CMAKE_WORKSPACE_NAME})

cm_find_package(CM)
include(CMDeploy)
include(CMSetupVersion)

cm_find_package(${CMAKE_WORKSPACE_NAME}_pbkdf)
cm_find_package(${CMAKE_WORKSPACE_NAME}_pubkey)
cm_find_package(${CMAKE_WORKSPACE_NAME}_codec)
cm_find_package(${CMAKE_WORKSPACE_NAME}_modes)

list(APPEND ${CURRENT_PROJECT_NAME}_PUBLIC_HEADERS
     include/nil/crypto3/ffi/ffi.h
     include/nil/crypto3/ffi/ffi_mp.h
     include/nil/crypto3/ffi/ffi_pkey.h
     include/nil/crypto3/ffi/ffi_rng.h
     include/nil/crypto3/ffi/ffi_util.h)

list(APPEND ${CURRENT_PROJECT_NAME}_UNGROUPED_SOURCES
     src/ffi.cpp
     src/ffi_block.cpp
     src/ffi_cipher.cpp
     src/ffi_hash.cpp
     src/ffi_kdf.cpp
     src/ffi_keywrap.cpp
     src/ffi_mac.cpp
     src/ffi_mp.cpp
     src/ffi_pk_op.cpp
     src/ffi_pkey.cpp
     src/ffi_pkey_algs.cpp
     src/ffi_rng.cpp)

list(APPEND ${CURRENT_PROJECT_NAME}_HEADERS
     ${${CURRENT_PROJECT_NAME}_PUBLIC_HEADERS})

list(APPEND ${CURRENT_PROJECT_NAME}_SOURCES
     ${${CURRENT_PROJECT_NAME}_UNGROUPED_SOURCES})

cm_setup_version(VERSION 0.1.0 PREFIX ${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME})

add_library(${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME}
            ${${CURRENT_PROJECT_NAME}_HEADERS}
            ${${CURRENT_PROJECT_NAME}_SOURCES})

set_target_properties(${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME} PROPERTIES
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED TRUE)

target_include_directories(${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME} PRIVATE
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>")

target_link_libraries(${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME} PUBLIC
                      ${CMAKE_WORKSPACE_NAME}::pbkdf
                      ${CMAKE_WORKSPACE_NAME}::pubkey
                      ${CMAKE_WORKSPACE_NAME}::codec
                      ${CMAKE_WORKSPACE_NAME}::modes)

cm_deploy(TARGETS ${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME} INCLUDE include/public include/private NAMESPACE ${CMAKE_WORKSPACE_NAME}::)

if(APPLE OR NOT ${CMAKE_TARGET_ARCHITECTURE} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR})
    set_target_properties(${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME} PROPERTIES
                          XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${APPLE_CODE_SIGN_IDENTITY}"
                          XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM}")
endif()