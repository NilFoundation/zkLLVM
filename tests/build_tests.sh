readarray test_examples < tests/tests_list.txt

if [ $1 != "make" ]  &&  [ $1 != "ninja" ]; then
  echo "Got $1 instead of make/ninja"
  echo "Build system must be entered as command line argument 1";
  exit 1
fi

if [[ $2 =~ ^-j[0-9]+$ ]]; then
  for test_example in ${test_examples[*]}; do
    $1 -C build ${test_example////_}_calculate_expected_res  $2;
    $1 -C build ${test_example////_}  $2;
  done
else
  echo "Got $2 as command line argument 2, must be number of cpu's"
  echo "(linux: -j\$(nproc), mac: : -j\$(sysctl -n hw.logicalcpu))"
fi
