# Standalone Clang usage

Now `zkLLVM clang` requires several essential arguments: 
```
-target assigner -Xclang -no-opaque-pointers -emit-llvm -S
```
Moreover, you need to use our custom [standard library](https://github.com/NilFoundation/zkllvm-stdlib). Library headers could be added with options:
```
-Izkllvm/libs/stdlib/libc/include -Izkllvm/libs/stdlib/libcpp
```
With these arguments, clang will generate `.ll` assembler as an output.
These assembly files could be linked with `llvm-link` binary (it’s also an artifact from zkLLVM project):
```
llvm-link -opaque-pointers=0 -o output.ll input1.ll input2.ll … inputX.ll
```
`output.ll` could be used further as an input for llvm-link. So you can link some intermediate targets first, and then use them for creating the final circuit.
The expected result that could be passed to `assigner` is a single `.ll` file that must contain exactly one function with `[[circuit]]` attribute, which is considered as an entry point of the circuit.
In case if you are going to use cmake as a build system, you could reuse our module [CircuitCompile.cmake](https://github.com/NilFoundation/zkllvm/blob/master/cmake/CircuitCompile.cmake).
