# Tools to help you build circuits

## Circuit validity checker

Circuit can be checked for validity by running `assigner` with the
`--check` flag. This will run the circuit through the evaluation process and throw an exception in case if it the constraints are not satisfied.

```bash
assigner -b main.bc -i main.inp -t assignment.tbl -c circuit.crct -e pallas --check
```

## Transpiler local proof generator

You can run local proving process by using `transpiler` tool in `gen-test-proof` mode.

```bash
transpiler -m gen-test-proof -i main.inp -c circuit.crct -t assignment.tbl -o .
```

Here `.crct` and `.tbl` files are the results of the circuit generation process by `assigner` tool. `.inp` file is the JSON input in the same format as `assigner` expects.

## Circuit debugger

ETA: Mid Dec 2023

## IDE extensions

ETA: Mid Dec 2023