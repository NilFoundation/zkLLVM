#!/bin/bash

            test_examples=( \
              "algebra/fields/pallas_add" \
              "algebra/fields/pallas_mul" \
            );

            for test_example in ${test_examples[*]}; do
              make -C build ${test_example////_}_calculate_expected_res  -j$(sysctl -n hw.logicalcpu);
              make -C build ${test_example////_}  -j$(sysctl -n hw.logicalcpu);
            done

            for test_example in ${test_examples[*]}; do
              for file in tests/inputs/algebra/fields/*.inp; do

              echo -n "Test ${test_example}, input $file:";
              ./build/tests/cpp/${test_example////_}_calculate_expected_res "$file" > expected.log
              ./build/bin/assigner/assigner  -b build/tests/cpp/${test_example////_}.ll -i "$file"  -t assignment.tbl -c circuit.crct -e pallas --check  > real.log
              diff -u expected.log real.log >diff.log;

              if [ $? -eq 0 ]; then
                echo -e "\033[32m succeeded \033[0m";
              else
                echo -e "\033[31m failed! \033[0m";
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