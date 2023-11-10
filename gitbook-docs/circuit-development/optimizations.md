# Effective circuit development

# Avoid overcomplicated data structures

Before generating the circuit, `assigner` will try to simplify the data structures and memory operations as much as possible. However, it is still recommended to avoid overcomplicated data structures. For example, if you have a `std::vector<std::vector<std::vector<int>>>` it is better to use a single `std::vector<int>` instead.

# Reduce amount of if-else statements

# Integer types are Galois field types

When developing the algorithm remember that all integer types by default are being treated as field types. Unless they are being used by an intrinsic function that takes bit size into account.

# Remember to use compile-time evaluations when possible

They are free and can significantly reduce the circuit size.

# The uniqueness is worth a lot

Due to PLONK-ish gates nature, $N$ unique instructions are more expensive than $N$ identical instructions of the similar size.

# Use const size containers when possible

In circuit form, const size containers have the same cost regardless of memory type they are using.

Before the code is transformed into the circuit - there is an additional layer of memory operations simplifying. That means that there is no difference in terms of circuits between `std::vector<int>(10)` and `std::array<int, 10>` - both these containers will be simplified to direct memory access.