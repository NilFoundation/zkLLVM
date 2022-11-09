Example of usage:
1. Building:
```bash
cmake -GNinja -B ${ASSIGNER_BUILD:-build} -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_PROJECTS=clang .
ninja -C ${ASSIGNER_BUILD:-build} clang assigner
```
2. Running:
```bash
${ASSIGNER_BUILD:-build}/libs/zkllvm/llvm/bin/clang samples/arithmetics.cpp -emit-llvm -c -O1 -o samples/arithmetics.bc
${ASSIGNER_BUILD:-build}/bin/assigner samples/arithmetics.bc
```
