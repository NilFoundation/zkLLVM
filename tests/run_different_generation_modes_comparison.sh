rm -r build/examples/cpp
rm -r build/1
rm -r build/2
/usr/bin/cmake --build "$(pwd)/build" --config Debug --target cpp_examples_generate_tbl_no_check --
/usr/bin/cmake --build "$(pwd)/build" --config Debug --target cpp_examples_generate_crct --
bash tests/copy_assigner_outputs.sh build/1
/usr/bin/cmake --build "$(pwd)/build" --config Debug --target cpp_examples_generate_both_no_check --
bash tests/copy_assigner_outputs.sh build/2

bash tests/compare_folders_content.sh build/1 build/2