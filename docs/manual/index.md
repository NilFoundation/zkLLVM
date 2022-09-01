# <span style='font-family: monospace'>=nil;</span> Crypto3 C++ Cryptography Suite {#mainpage}

@tableofcontents

Crypto3 cryptography suite's purpose is:
1. To provide a secure, fast and architecturally clean C++ generic cryptography schemes implementation.
2. To provide a developer-friendly, modular suite, usable for novel schemes implementation and further
 extension.
3. To provide a Standard Template Library-alike C++ interface and concept-based architecture implementation.

The libraries are designed to be state of the art, highly performant and providing a one-stop solution for 
all cryptographic operations. They are supported on all operating systems (*nix, windows, macOS) 
and architectures(x86/ARM). See the quickstart guide to set up the environment and run some examples.

See contribution guidelines if you wish to help develop the project.


## Crypto3 Directory structure
<pre>
root
├── cmake: Cmake sub-module with helper functions/macros to build crypto3 library mono-repository
├── libs: all directories added as submodules which are independent projects.
│   ├── algebra: algebraic constructions being used mostly for public-key schemes
│   ├── block: block ciphers
│   ├── blueprint: zk-circuit definitions 
│   ├── codec: encoding/decoding algorithms
│   ├── containers: //TODO
│   ├── ffi: //TODO
│   ├── hash: hashing algorithms
│   ├── kdf: key derivation functions 
│   ├── mac: message authentication codes
│   ├── marshalling: marshalling libraries for types in crypto3 library
│   ├── math: set of Fast Fourier Transforms evaluation algorithms and Polynomial Arithmetics
│   ├── modes: cipher modes
│   ├── multiprecision:provides integer, rational, floating-point, complex and interval number types. 
│   ├── passhash: password hashing operations 
│   ├── pbkdf: password based key derivation functions
│   ├── pkmodes: threshold, aggregation modes for schemes defined within pubkey library
│   ├── pkpad: //TODO
│   ├── pubkey: pubey signing APIs
│   ├── random: randomisation primitives 
│   ├── stream: stream ciphers
│   ├── vdf:verifiable delay functions 
│   ├── zk: zk cryptography schemes
</pre>


### Modules 
Cryptography suite contains multiple modules from trivial encoding (e.g. base64), encryption, hashing 
to complex protocols (e.g. threshold schemes, zero-knowledge protocol or verifiable delay functions).

Detailed list of modules with tutorials is available at [Modules page](modules.html).

### Usage

Cryptography suite can be used as follows:

1. Generic usage.
2. Selective module usage.

The suite is used as a header-only and is currently statically linked. Future versions will allow dynamic linking. 

#### Generic Usage
Generic usage-intended cryptography suite comprises modules available at [GitHub =nil; Crypto3 Team Repositories](https://github.com/orgs/NilFoundation/teams/nil-crypto3/repositories). 
Some of them are not for standalone usage, some of them are. So there is a suite mono repository available at
 [Crypto3 GitHub](https://github.com/nilfoundation/crypto3.git). mono repository includes all the modules
available as submodules emplaced in `libs` directory. A developer can thus include this as a submodule in their 
project and would not need to resolve dependencies of each module. See [crypto3-scaffold](https://github.com/NilFoundation/crypto3-scaffold) as an example of usage. 

Generic-purposed suite uses CMake as build system with
[cmake-modules](https://github.com/BoostCMake/cmake-modules.git) CMake library used for easier
in-project targets management.
  
### Selective module usage

Developer can select to include a one or more modules to reduce the sources of resulting project and dependencies tree height. This however
does require the developer to manually resolve all required dependencies.


## Getting started

Navigate to [quickstart]() to set up and configure the environment or [here](modules.html) for detailed explanations per module. 