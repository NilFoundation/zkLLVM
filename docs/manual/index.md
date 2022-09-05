# <span style='font-family: monospace'>=nil;</span> Crypto3 C++ Cryptography Suite {#mainpage}

@tableofcontents

Crypto3 cryptography suite's purpose is:
1. To provide a secure, fast and architecturally clean C++ generic cryptography schemes implementation.
2. To provide a developer-friendly, modular suite, usable for novel schemes implementation and further
 extension.
3. To provide a Standard Template Library-alike C++ interface and concept-based architecture implementation.

Libraries are designed to be state of the art, highly performant and providing a one-stop solution for 
all cryptographic operations. They are supported on all operating systems (*nix, windows, macOS) 
and architectures(x86/ARM).


See [contribution](contributing.md) guidelines if you wish to contribute to the project.

## Modules
Cryptography suite contains multiple modules from trivial encoding (e.g. base64), encryption, hashing
to complex protocols (e.g. threshold schemes, zero-knowledge protocol or verifiable delay functions).

Detailed list of modules with tutorials is available at [Modules page](modules.html).

## Project structure
```
root
├── cmake: cmake sub-module with helper functions/macros to build crypto3 library umbrella-repository
├── docs: documentation , tutorials and guides
├── libs: all directories added as submodules which are independent projects.
│   ├── algebra: algebraic operations and structures being used for elliptic-curve cryptography
│   ├── block: block ciphers
│   ├── blueprint: components and circuits for zk schemes
│   ├── codec: encoding/decoding algorithms
│   ├── containers:containers and generic commitment schemes for accumulating data, includes Merkle Tree
│   ├── hash: hashing algorithms
│   ├── kdf: key derivation functions 
│   ├── mac: message authentication codes
│   ├── marshalling: marshalling libraries for types in crypto3 library
│   ├── math: set of Fast Fourier Transforms evaluation algorithms and Polynomial Arithmetics
│   ├── modes: cipher modes
│   ├── multiprecision: integer, rational, floating-point, complex and interval number types. 
│   ├── passhash: password hashing operations 
│   ├── pbkdf: password based key derivation functions
│   ├── pkmodes: threshold, aggregation modes for public key schemes
│   ├── pkpad: padding module for public key schemes
│   ├── pubkey: pubkey signing APIs
│   ├── random: randomisation primitives 
│   ├── stream: stream ciphers
│   ├── vdf:verifiable delay functions 
│   ├── zk: zk cryptography schemes
```


## Usage

Cryptography suite can be used as follows:

1. Generic.
2. Selective.

The suite is used as a header-only and is currently statically linked. Future versions will allow dynamic linking. 

#### Generic
Generic usage of cryptography suite consists of all modules available at 
[GitHub =nil; Crypto3 Team Repositories](https://github.com/orgs/NilFoundation/teams/nil-crypto3/repositories). 
An umbrella-repository of it is available at  [Crypto3 GitHub](https://github.com/nilfoundation/crypto3.git). Modules
are added as submodules emplaced in `libs` directory. A developer can thus add this  
project as a submodule and would not need to resolve dependencies. See [crypto3-scaffold](https://github.com/NilFoundation/crypto3-scaffold) as an example of usage. 

### Selective
Developer can select to include a one or more modules to reduce the sources of resulting project and dependencies tree height. This however
does require the developer to manually resolve all required dependencies and stay upto date regarding
compatibilities across modules.


## Next steps
 <!--TODO Link generated via doxygen -->
Navigate to quickstart guide to set up and configure the environment or [here](modules.html) for detailed explanations per module. 