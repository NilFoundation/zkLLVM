# Aggregated prover mode

Placeholder proof system supports recursive/aggregated mode. In this mode, proof-producer will run the proof generation process for each of the sub-proofs and then aggregate them into a single proof.

This mode is useful when you have a complex circuit that can be split into multiple sub-circuits. For example, you can split the circuit into two parts: one part will be responsible for the data processing and the second part will be responsible for the data storage. In this case, you can write the circuit algorithm code in aggregated manner and then designate it to multiple provers. Each prover will be responsible for a single sub-circuit.

## How to trigger aggregation

For use aggregation mode need to define which prover should execute each blocks of code by adding `#pragma zk_multi_prover <PROVER_INDEX>`.
PROVER_INDEX may have value [0, MAX_NUM_PROVERS). The pragma affect follow code block enclosed in `{...}`.
For example:
```
a = b + c;
a = a * 10;
a = a + c * 10;
```
will be executed by one prover with index 0
```
a = b + c;
#pragma zk_multi_prover 1
{
    a = a * 10;
    a = a + c * 10;
}

```
will be executed by 2 provers: first line - by prover with index 0, second and third - by prover with index 1.

## How to run local prover in aggregated mode