rm -r build/examples/cpp
/usr/bin/cmake --build "$(pwd)/build" --config Debug --target cpp_examples_generate_tbl --
/usr/bin/cmake --build "$(pwd)/build" --config Debug --target cpp_examples_generate_crct --
bash tests/copy_assigner_outputs.sh build/1
/usr/bin/cmake --build "$(pwd)/build" --config Debug --target cpp_examples_generate_crct --
bash tests/copy_assigner_outputs.sh build/2

bash tests/compare_folders_content.sh build/1 build/2