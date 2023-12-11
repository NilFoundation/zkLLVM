set(${CURRENT_PROJECT_NAME}_HEADERS)

set(INPUTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inputs)
set(PRIVATE_INPUTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/private_inputs)

function(assign_ir target binary_name input curve_type)

   if(EXISTS ${PRIVATE_INPUTS_DIR}/${input})
      set(minus_p -p)
      set(priv_inp ${PRIVATE_INPUTS_DIR}/${input})
   endif()

   add_custom_target(${target}_assign
                      COMMAND $<TARGET_FILE:assigner>
                      -b ${binary_name}
                      -i ${INPUTS_DIR}/${input}
                      ${minus_p} ${priv_inp}
                      -c circuit_${target}.crct
                      -t assignment_${target}.tbl
                      -e ${curve_type}
                      --check
                      DEPENDS ${target} ${INPUTS_DIR}/${input} $<TARGET_FILE:assigner>
                      COMMAND_EXPAND_LISTS
                      VERBATIM)
endfunction()

function(run_expected_res_calculation target input_path input_name)
   add_custom_target(${target}_run_${input_name}
   COMMAND $<TARGET_FILE:${target}> ${INPUTS_DIR}/${input_path}/${input_name}.inp
   DEPENDS ${target} ${INPUTS_DIR}/${input}
   COMMAND_EXPAND_LISTS
   VERBATIM)
endfunction()


add_subdirectory(cpp)
