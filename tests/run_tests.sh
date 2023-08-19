#!/bin/bash

readarray test_examples < tests/tests_list.txt
readarray input_dirs_list < tests/input_dirs_list.txt
exit_code=0

            # shift_examples=( \
            #   "algebra/integers/bit_operations/left_shift" \
            #   "algebra/integers/bit_operations/right_shift" \
            # );

            # comparison_examples=( \
            #   "algebra/integers/comparison/more" \
            #   "algebra/integers/comparison/less" \
            # );

for i in ${!test_examples[*]}; do
  test_example=${test_examples[i]//$'\n'/}
  parent_dir=$(dirname "$test_example")

  echo test_example: $test_example
  echo parent_dir: $parent_dir

  inputs_directory=${input_dirs_list[i]//$'\n'/}
  # ls ${inputs_directory}

  qwerty=${inputs_directory//$'\n'/}
  for file in tests/inputs/$parent_dir/*.inp; do

  echo -n "Test ${test_example}, input $file: ";
  ./build/bin/assigner/assigner  -b build/tests/cpp/${test_example////_}.ll -i "$file"  -t assignment.tbl -c circuit.crct -e pallas --check  > real.log
  if [ $? -ne 0 ]; then
    exit_code=1
    echo -e "\033[31m Assigner failed! \033[0m";
    continue
  fi

  ./build/tests/cpp/${test_example////_}_calculate_expected_res "$file" > expected.log
  if [ $? -ne 0 ]; then
    exit_code=1
    echo -e "\033[31m Calculation of expected result failed! \033[0m";
    continue
  fi

  # echo "16" > real.log
  diff -u expected.log real.log >diff.log;

  if [ $? -eq 0 ]; then
    echo -e "\033[32m succeeded \033[0m";
  else
    exit_code=1
    echo -e "\033[31m Test failed! Real result fiffers from expected result. \033[0m";
    echo -e "\nexpected result:";
    cat expected.log;
    echo -e "\nreal circuit output:";
    cat real.log;
    echo -e "\ninput:";
    cat "$file";
    echo -e "\n--------------------------------------------------------------------------------";
  fi
  done
done

rm real.log expected.log diff.log

exit $exit_code