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
${ASSIGNER_BUILD:-build}/libs/circifier/llvm/bin/clang samples/sha512.cpp -emit-llvm -c -O1 -o samples/sha512.bc
${ASSIGNER_BUILD:-build}/bin/assigner samples/sha512.bc -i samples/sha512.inp
```
