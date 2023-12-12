readarray expected_results < tests/expected_results_list.txt
readarray circuit_outputs < tests/circuits_outputs_list.txt

exit_code=0
at_least_one_test_launched=0

for i in ${!expected_results[*]}; do
  expected_res="build/tests/cpp/${expected_results[i]//$'\n'/}"
  circuit_output="build/tests/cpp/${circuit_outputs[i]//$'\n'/}"

  echo ""
  echo current test: $circuit_output
  # echo "${pwd}"



  # qwe="build/tests/cpp/circuit_output_algebra_curves_25519_arithmetics_add_rand_rand_2.txt"
  qwe=$circuit_output
  # if [[ -e "pwd/$expected_res" ]]; then
  # if [[ -e "build/tests/cpp/" ]]; then
  if [[ ! -e $circuit_output ]]; then
    echo -e "\033[31m Circuit output file does not exist! \033[0m";
    exit_code=1

      if [[ ! -e $expected_res ]]; then
        echo -e "\033[31m expected result file does not exist! \033[0m";
        exit_code=1
        continue
      fi
    continue
  fi


    diff -u $expected_res $circuit_output >diff.log;

    if [ $? -eq 0 ]; then
      echo -e "\033[32m succeeded \033[0m";
    else
      exit_code=1
      echo -e "\033[31m Test failed! Real result differs from expected result. \033[0m";
      echo -e "\nexpected result:";
      cat $expected_res;
      echo -e "\nreal circuit output:";
      cat $circuit_output;
      echo -e "\ndiff:";
      cat diff.log;
      # echo -e "\ninput:";
      # cat "$file";
      echo -e "\n--------------------------------------------------------------------------------";
    fi
    at_least_one_test_launched=1
    rm $circuit_output $expected_res diff.log
done


if [ $at_least_one_test_launched -eq 0 ]; then
  exit 1
else
  exit $exit_code
fi