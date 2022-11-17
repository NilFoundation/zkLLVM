# =nil; Foundation's zkLLVM Circuit Compiler

## Usage:

1. CMake configure:
```bash
cmake -GNinja -B ${ASSIGNER_BUILD:-build} -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_PROJECTS=clang .
```

2. Build clang, assigner, and examples:
```bash
ninja -C ${ASSIGNER_BUILD:-build} clang assigner circuit_examples -j$(nproc)
```

3. Run:
```bash
${ASSIGNER_BUILD:-build}/bin/assigner/assigner ${ASSIGNER_BUILD:-build}/examples/sha512_example.ll -i examples/sha512.inp
```
