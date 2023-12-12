set(${CURRENT_PROJECT_NAME}_HEADERS)

set(INPUTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inputs)
set(PRIVATE_INPUTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/private_inputs)

function(assign_ir target binary_name input curve_type max_num_provers_amount)
   message(private_input: ${max_num_provers_amount})
   if(NOT max_num_provers_amount EQUAL 0)
      set(max_num_provers_flag --max-num-provers)
      set(max_num_provers_amount ${max_num_provers_amount})
   endif()

   if(EXISTS ${PRIVATE_INPUTS_DIR}/${input})
      set(minus_p -p)
      set(priv_inp ${PRIVATE_INPUTS_DIR}/${input})
   endif()

   file(APPEND ../circuits_outputs_list.txt circuit_output_${target}.txt\n)
   add_custom_target(${target}_assign
                      COMMAND $<TARGET_FILE:assigner>
                      -b ${binary_name}
                      -i ${INPUTS_DIR}/${input}
                      ${minus_p} ${priv_inp}
                      -c circuit_${target}.crct
                      -t assignment_${target}.tbl
                      -e ${curve_type}
                      --check
                      -f dec
                      ${max_num_provers_flag} ${max_num_provers_amount}
                      > circuit_output_${target}.txt
                      DEPENDS ${target} ${INPUTS_DIR}/${input} $<TARGET_FILE:assigner>
                      COMMAND_EXPAND_LISTS
                      VERBATIM)
endfunction()

function(run_expected_res_calculation target input_path input_name)
   file(APPEND ../expected_results_list.txt expected_res_${target}_${input_name}.txt\n)

   add_custom_target(${target}_run_${input_name}
   COMMAND $<TARGET_FILE:${target}> ${INPUTS_DIR}/${input_path}/${input_name}.inp > expected_res_${target}_${input_name}.txt
   DEPENDS ${target} ${INPUTS_DIR}/${input}
   COMMAND_EXPAND_LISTS
   VERBATIM)
   endfunction()


add_subdirectory(cpp)
