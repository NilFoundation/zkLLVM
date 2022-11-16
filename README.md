# =nil; Foundation's zkLLVM Circuit Compiler

Example of usage:
1. CMake configure:
```bash
cmake -GNinja -B ${ASSIGNER_BUILD:-build} -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_PROJECTS=clang .
```

2. Build clang and assigner:
```bash
ninja -C ${ASSIGNER_BUILD:-build} clang assigner -j$(nproc)
```

2. Running:
```bash
${ASSIGNER_BUILD:-build}/libs/circifier/llvm/bin/clang examples/sha512.cpp -emit-llvm -c -O1 -o examples/sha512.bc
${ASSIGNER_BUILD:-build}/bin/assigner examples/sha512.bc -i examples/sha512.inp
```
