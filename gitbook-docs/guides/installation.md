---
description: Compile zkLLVM artifacts
---

# Installation

## Clone repository

Clone the repository and all sub-modules

```
git clone --recurse-submodules git@github.com:NilFoundation/zkllvm.git
cd zkllvm
```

## **Configure cmake**&#x20;

```bash
cmake -GNinja -B ${ASSIGNER_BUILD:-build} -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_PROJECTS=clang .
```

## **Build clang**&#x20;

```bash
ninja -C ${ASSIGNER_BUILD:-build} assigner -j$(nproc)
```

## **Build assigner**

```
ninja -C ${ASSIGNER_BUILD:-build} assigner -j$(nproc)
```

## Test

```bash
${ASSIGNER_BUILD:-build}/libs/circifier/llvm/bin/clang samples/sha512.cpp -emit-llvm -c -O1 -o samples/sha512.bc
${ASSIGNER_BUILD:-build}/bin/assigner samples/sha512.bc -i samples/sha512.inp
```
