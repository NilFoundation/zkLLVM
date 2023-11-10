# Aggregated prover mode

Placeholder proof system supports recursive/aggregated mode. In this mode, proof-producer will run the proof generation process for each of the sub-proofs and then aggregate them into a single proof.

This mode is useful when you have a complex circuit that can be split into multiple sub-circuits. For example, you can split the circuit into two parts: one part will be responsible for the data processing and the second part will be responsible for the data storage. In this case, you can write the circuit algorithm code in aggregated manner and then designate it to multiple provers. Each prover will be responsible for a single sub-circuit.

## How to trigger aggregation

## How to run local prover in aggregated mode