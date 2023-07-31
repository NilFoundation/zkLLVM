cm_project(test_native_field_pallas_add WORKSPACE_NAME ${CMAKE_WORKSPACE_NAME} LANGUAGES ASM C CXX)
set(${CURRENT_PROJECT_NAME}_HEADERS)

list(APPEND ${CURRENT_PROJECT_NAME}_SOURCES
     cpp/algebra/fields/native_pallas_field_add.cpp)

add_executable(${CURRENT_PROJECT_NAME}
     ${${CURRENT_PROJECT_NAME}_HEADERS}
     ${${CURRENT_PROJECT_NAME}_SOURCES})

     target_link_libraries(${CURRENT_PROJECT_NAME}
     crypto3::algebra)

set(INPUTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inputs)

function(assign_ir target binary_name input)
    add_custom_target(${target}_assign
                      COMMAND $<TARGET_FILE:assigner> -b ${binary_name} -i ${INPUTS_DIR}/${input} -c circuit_${target}.crct -t assignment_${target}.tbl -e pallas --check
                      DEPENDS ${target} ${INPUTS_DIR}/${input} $<TARGET_FILE:assigner>
                      COMMAND_EXPAND_LISTS
                      VERBATIM)
endfunction()

function(gen_proof target input)
    add_custom_target(${target}_prove
                      COMMAND $<TARGET_FILE:transpiler> -m gen-test-proof -i ${INPUTS_DIR}/${input} -c circuit_${target}.crct -t assignment_${target}.tbl -o .
                      DEPENDS ${target}_assign $<TARGET_FILE:transpiler>
                      COMMAND_EXPAND_LISTS
                      VERBATIM)
endfunction()

add_subdirectory(cpp)
