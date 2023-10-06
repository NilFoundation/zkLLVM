readarray test_examples < tests/tests_list.txt
exit_code=0
at_least_one_test_launched=0

for i in ${!test_examples[*]}; do
  test_example=${test_examples[i]//$'\n'/}
  parent_dir=$(dirname "$test_example")

  if [ ! -d "tests/faulty_inputs/$parent_dir" ]; then
    continue
  fi

  echo ""
  echo test_example: $test_example

  for file in tests/faulty_inputs/$parent_dir/*.inp; do

    echo -n "input $file: ";
    ./build/bin/assigner/assigner  -b build/tests/cpp/${test_example////_}.ll -i "$file"  -t assignment.tbl -c circuit.crct -e pallas
    if [ $? -ne 0 ]; then
      exit_code=1
      echo -e "\033[31m Assigner failed before constraint check. Test failed! \033[0m";
      continue
    fi

    ./build/bin/assigner/assigner  -b build/tests/cpp/${test_example////_}.ll -i "$file"  -t assignment.tbl -c circuit.crct -e pallas --check
    if [ $? -eq 0 ]; then
      exit_code=1
      echo -e "\033[31m Circuit must not be satisfied, but it is. Test failed! \033[0m";
      continue
    else
      echo -e "\033[32m Circuit is not satisfied, as expected. Success! \033[0m";
    fi
    at_least_one_test_launched=1
  done
done

if [ $at_least_one_test_launched -eq 0 ]; then
  exit 1
else
  exit $exit_code
fi