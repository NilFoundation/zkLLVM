---
description: Install zkLLVM toolchain
---

# Binary Installation

zkLLVM is distributed as a deb package, so you can install it using the following commands:

```bash
echo 'deb [trusted=yes]  http://deb.nil.foundation/ubuntu/ all main' >>/etc/apt/sources.list
apt update
apt install -y zkllvm cmake libboost-all-dev
```

For Ubuntu, use the following commands:

```bash
sudo bash -c "echo 'deb [trusted=yes]  http://deb.nil.foundation/ubuntu/ all main' >>/etc/apt/sources.list"
sudo apt update
sudo apt install -y zkllvm cmake libboost-all-dev
```

# Installation from sources

## Clone repository

Clone the repository and all sub-modules

```
git clone --recurse-submodules git@github.com:NilFoundation/zkllvm.git
cd zkllvm
```

## **Configure cmake**

If you wish to generate IR (Intermediate representation) in binary format (\*.bc). This format is not compatible with the proof market currently.

```bash
cmake -G "Unix Makefiles" -B ${ZKLLVM_BUILD:-build} -DCMAKE_BUILD_TYPE=Release .
```

If you wish to generate IR (Intermediate representation) in text format (\*.ll). This format is compatible with the proof market.

```
cmake -G "Unix Makefiles" -B ${ZKLLVM_BUILD:-build} -DCMAKE_BUILD_TYPE=Release -DCIRCUIT_ASSEMBLY_OUTPUT=TRUE .
```

## **Build clang /assigner**

```bash
make -C ${ZKLLVM_BUILD:-build} assigner clang -j$(nproc)
```

## Examples

### Build Circuit IR

This builds some samples we have in the `examples` directory

```bash
make -C ${ZKLLVM_BUILD:-build} circuit_examples -j$(nproc) 
```

### Generate Execution trace

This generates an execution trace for the arithmetic example built.

```
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b ${ZKLLVM_BUILD:-build}/examples/arithmetics_example.bc -i examples/arithmetics.inp -t assignment.tbl -c circuit.crct
```

Where

Inputs:

* _**b**_: Binary representation of the circuit
* _**i**_: Public inputs for the circuit

Output:

* _**t**_: Execution trace
* _**c**_: Pre-processed constraints
