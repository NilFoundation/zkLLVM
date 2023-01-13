# =nil; Foundation's Cryptography Suite
[![Build docs](https://github.com/NilFoundation/crypto3/actions/workflows/build_docs.yaml/badge.svg)](https://github.com/NilFoundation/crypto3/actions/workflows/build_docs.yaml) 
[![Discord](https://img.shields.io/discord/969303013749579846.svg?logo=discord&style=flat-square)](https://discord.gg/KmTAEjbmM3)
[![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=flat-square&logo=telegram&logoColor=dark)](https://t.me/nilfoundation)

Crypto3 cryptography suite's purpose is:
1. To provide a secure, fast and architecturally clean C++ generic cryptography schemes implementation.
2. To provide a developer-friendly, modular suite, usable for novel schemes implementation and further
   extension.
3. To provide a Standard Template Library-alike C++ interface and concept-based architecture implementation.

Libraries are designed to be state of the art, highly performant and providing a one-stop solution for
all cryptographic operations. They are supported on all operating systems (*nix, windows, macOS)
and architectures(x86/ARM).

Developed by [=nil; Crypto3](https://crypto3.nil.foundation) and supported by [=nil; Foundation](https://nil.foundation).

Rationale, tutorials and references are available [here](https://crypto3.nil.foundation/projects/crypto3)
 
## Contents
1. [Repository Structure](#repository-structure)
2. [Installation](#installation)
3. [Usage](#usage)
3. [Contributing](#contributing)
4. [Community](#community)

## Repository Structure
This repository is an umbrella-repository for the whole suite. Single-purposed libraries repositories (e.g. [block
](https://github.com/nilfoundation/block) or [hash](https://github.com/nilfoundation/hash)) are not advised to be
used outside this suite or properly constructed CMake project and should be handled with great care.
```
root
├── cmake: cmake sub-module with helper functions/macros to build crypto3 library umbrella-repository
├── docs: documentation , tutorials and guides
├── libs: all directories added as submodules which are independent projects.
│   ├── algebra: algebraic operations and structures being used for elliptic-curve cryptography
│   ├── block: block ciphers
│   ├── blueprint: components and circuits for zk schemes
│   ├── codec: encoding/decoding algorithms
│   ├── containers: containers and generic commitment schemes for accumulating data, includes Merkle Tree
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
│   ├── vdf: verifiable delay functions 
│   ├── zk: zk cryptography schemes
```


## Installation
### Dependencies

- [clang](https://clang.llvm.org/) (>= 11.0)/GCC (>= 10.0)/MSVC (>= 14.20)
- [cmake](https://cmake.org) (>= 3.6)
- [boost](https://boost.org) (>= 1.76)

### Clone & Build

```
git clone --recurse-submodules https://github.com/nilfoundation/crypto3.git 
cd crypto3 && mkdir build && cd build
cmake ..
make tests
```



## Usage

Cryptography suite can be used as follows:

1. Generic.
2. Selective.

The suite is used as a header-only and is currently statically linked. Future versions will allow dynamic linking.

#### Generic
Generic usage of cryptography suite consists of all modules available at
[GitHub =nil; Crypto3 Team Repositories](https://github.com/orgs/NilFoundation/teams/nil-crypto3/repositories).
This is an umbrella-repository where  Modules
are added as submodules emplaced in `libs` directory. A developer can thus add this  
project as a submodule and would not need to resolve dependencies. See [crypto3-scaffold](https://github.com/NilFoundation/crypto3-scaffold) as an example of usage.

The generic module can be added to your c++ project as follows

``` git submodule add https://github.com/NilFoundation/crypto3.git <dir>```

### Selective
Developer can select to include a one or more modules to reduce the sources of resulting project and dependencies tree height. This however
does require the developer to manually resolve all required dependencies and stay upto date regarding
compatibilities across modules.

Example of such embedding is =nil; Foundation's [Actor Library](https://github.com/nilfoundation/actor). It uses only
[hashes](https://github.com/nilfoundation/hash) so the dependency graph requires
for the project to submodule [block ciphers library](https://github.com/nilfoundation/block) and optional
[codec library](https://github.com/nilfoundation/codec) for testing purposes. So,
the root Actor repository has only related libraries submoduled:
[block](https://github.com/nilfoundation/mtl/libs/block),
[codec](https://github.com/nilfoundation/mtl/libs/codec) and
[hash](https://github.com/nilfoundation/mtl/hash).

Selective modules can be added to your project as follows:

``` git submodule add https://github.com/NilFoundation/crypto3-<lib>.git <dir>```


## Contributing

See [contributing](./docs/manual/contributing.md) for contribution guidelines.

## Support

This cryptography suite is authored by [=nil; Crypto3](https://crypto3.nil.foundation) team, so you can contact it
 several ways:
 * E-Mail. Just drop a line to [nemo@nil.foundation](mailto:nemo@nil.foundation).
 * Telegram Group. Join our Telegram group [@nilfoundation](https://t.me/nilfoundation) and ask any question in there.
 * Discord [channel](https://discord.gg/KmTAEjbmM3) for discussions.
 * Issue. Issue which does not belong to any particular module (or you just don't know where to put it) can be
  created in this repository. The team will answer that.
 * Discussion Topic (proposal, tutorial request, suggestion, etc). Would be happy to discuss that in the repository's GitHub [Discussions](https://github.com/NilFoundation/crypto3/discussions)

## Licence

The software is provided under [MIT](LICENSE) Licence.

