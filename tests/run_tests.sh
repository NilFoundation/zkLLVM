readarray test_examples < tests/tests_list_$1.txt
exit_code=0
at_least_one_test_launched=0

for i in ${!test_examples[*]}; do
  test_example=${test_examples[i]//$'\n'/}
  parent_dir=$(dirname "$test_example")

  echo ""
  echo test_example: $test_example

  for file in tests/inputs/$parent_dir/*.inp; do

    echo -n "input $file: ";

    if [[ $test_example == *"multi_provers"* ]]; then
      ./build/bin/assigner/assigner  -b build/tests/cpp/${test_example////_}.ll -i "$file"  -t assignment.tbl -c circuit.crct -e $1 --check -o print_circuit_output --max-num-provers 2  > real.log
    else
      ./build/bin/assigner/assigner  -b build/tests/cpp/${test_example////_}.ll -i "$file"  -t assignment.tbl -c circuit.crct -e $1 --check -o print_circuit_output  > real.log
    fi

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

    diff -u expected.log real.log >diff.log;

    if [ $? -eq 0 ]; then
      echo -e "\033[32m succeeded \033[0m";
    else
      exit_code=1
      echo -e "\033[31m Test failed! Real result differs from expected result. \033[0m";
      echo -e "\nexpected result:";
      cat expected.log;
      echo -e "\nreal circuit output:";
      cat real.log;
      echo -e "\ninput:";
      cat "$file";
      echo -e "\n--------------------------------------------------------------------------------";
    fi
    at_least_one_test_launched=1
  done
done

rm real.log expected.log diff.log

if [ $at_least_one_test_launched -eq 0 ]; then
  exit 1
else
  exit $exit_code
fi