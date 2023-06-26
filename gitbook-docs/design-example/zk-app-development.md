# zk-Application circuit development

Development of zk-requiring applications usualy consist of several parts.

One of the major things to focus on is the circuit logic and circuit implementation. You need to understand, what exactly you want to prove in your circuit.

{% hint style="info" %}
zk-SNARKS/STARKS and Circuits are not a silver bullet. It is a powerfull technology, which can give you huge benefits, but it is not a magic. If you write your circuit in a wrong way, you can get a huge overhead, or even get a circuit, which allows to bypass the application logic. You obviously don't want that - so be careful and study the basics before rushing into the development.
{% endhint %}

If you are new into circuit development I would recommend you to learn zk-SNARKs/STARKs basics and then start your development with the [Getting Started](../circuit-development/circuit-generation.md) section.

For more advanced usage - check out the [Limitation](../circuit-development/limitations.md) and [Effective circuit development](../circuit-development/optimizations.md) sections.

## Circuit example for a zk-Bridge

You can find a full example of a zk-Bridge circuit in the [zkllvm examples](https://github.com/NilFoundation/zkllvm/blob/master/examples/zkbridge.cpp) directory.

If you want to learn more about how this example works and how to extend its algorithm, check out the [Tutorials](../tutorials/01-hashes.md) section.