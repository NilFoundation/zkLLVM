# =nil; Foundation's Cryptography Suite (libcrypto3)

Modern cryptography suite built in C++ with concept-based architecture.

Development repository is https://github.com/nilfoundation/crypto3.
 
Rationale, detailed tutorials and references are available at https://crypto3.nil.foundation/projects/crypto3.
 
Developed by [=nil; Crypto3](https://crypto3.nil.foundation) and supported by [=nil; Foundation](https://nil.foundation).

All the communication is being processed via [several methods](#contacts).

This repository is an umbrella-repository for the whole suite. Particular-purposed libraries repositories (e.g. [block
](https://github.com/nilfoundation/block) or [hash](https://github.com/nilfoundation/hash)) are not supposed to be
 used outside the suite or properly constructed CMake project and should be handled with great care.

## Contents
1. [Repository Structure](#repository-structure)
2. [Repository Contents](#repository-contents)
3. [Building](#building)
4. [Contributing](#contributing)
5. [Community](#community)

## Repository Structure

This repository structure varies depending on particular usage purpose:

### Generic cryptography suite usage.

Useful for submoduling with projects with CMake build system or for usual build and install routine.

This case gets covered with submodule-based header-only superproject including all the stable libraries available. 
Every library is emplaced in ```libs/%library_name%``` as submodule. Depends on Boost (https://boost.org).

Handled by branches ```master```, ```develop``` and ```#issue-name``` branches (uses git-flow) along with semantic versioning-tagged ```master``` branch commits (e.g. ```1.2.34```).

Uses CMake build system.

### Custom libraries set usage. 

Suite is designed in a way for the every particular user could include and exclude particular libraries and features depending on a usecase.

This could be useful for keeping a codebase small and tiny along with keeping submodules tree height <= 1. 
This could be covered by several ways:
    
1. Taking a look at dependency graph of each library (included in every library repository description) and embedding the particular set of libraries in the user's project.
     
Fine example of such embedding is =nil; Foundation's Actor Library 
(https://github.com/nilfoundation/actor). It uses only 
[hashes](https://github.com/nilfoundation/hash) so the dependency graph requires 
for the project to submodule [block ciphers library](https://github.com/nilfoundation/block) and optional 
[codec library](https://github.com/nilfoundation/codec) for testing purposes. So, 
the root Actor repository has only related libraries submoduled: 
[block](https://github.com/nilfoundation/mtl/libs/block), 
[codec](https://github.com/nilfoundation/mtl/libs/codec) and 
[hash](https://github.com/nilfoundation/mtl/hash).
        
## Repository Contents

This particular repository contains all of the modules available within the suite as submodules emplaced in
 ```libs``` directory. This is called "Modularized Version". 

### Modularized Version

Modularized version contains all of the modules available emplaced in ```libs``` directory:

* Algebra (https://github.com/nilfoundation/crypto3-algebra.git). Curves and finite fields operations. Based on Multiprecision.
* Block Ciphers (https://github.com/nilfoundation/crypto3-block.git). Block ciphers. Encryption/decryption.
* Codecs (https://github.com/nilfoundation/crypto3-codec.git). Encoding/decoding (e.g. with base64).
* Hashes (https://github.com/nilfoundation/crypto3-hash.git). Hashing.
* Key Derivation Functions (https://github.com/nilfoundation/crypto3-kdf.git) (Drafty).
* Message Authentication Codes (https://github.com/nilfoundation/crypto3-mac.git) (Drafty)
* Cipher Modes (https://github.com/nilfoundation/crypto3-modes.git) (Drafty)
* Multiprecision (https://github.com/nilfoundation/crypto3-multiprecision.git). Multiprecision library enhanced with cryptography-required constructions and techniques. 
* Passhash (https://github.com/nilfoundation/crypto3-passhash.git) (Drafty)
* Public Key Schemes (https://github.com/nilfoundation/crypto3-pubkey.git). Single-participant public-key schemes.
* Public Key Schemes Modes (https://github.com/nilfoundation/crypto3-pkmodes.git). Threshold/MPC enhancements to public-key schemes.
* Stream Ciphers (https://github.com/nilfoundation/crypto3-stream.git) (Drafty)
* Random (https://github.com/nilfoundation/crypto3-random.git) (Drafty)
* Verifiable Delay Functions (https://github.com/nilfoundation/crypto3-vdf.git). Wesolowski VDF, Pietrzak VDF, Chia VDF.
* Zero-Knowledge Proofs (https://github.com/nilfoundation/crypto3-zk.git). Pedersen/Schnorr/Fiat-Shamir schemes, zk-SNARKs, zk-STARKs, Bulletproofs, etc.

## Building
### Dependencies

Clang (>= 11.0)/GCC (>= 10.0)/MSVC (>= 14.20)

[CMake](https://cmake.org) (>= 3.6)

[Boost](https://boost.org) (>= 1.76)

### Clone & Build

```
git clone --recurse-submodules https://github.com/nilfoundation/crypto3.git 
cd crypto3 && mkdir build && cd build
cmake ..
make tests
```

## Contributing

### Issues

Preferred way to submit an issue is to submit that to the particular module you think the trouble is in. In case you are not sure, submit an issue to this (master) repository, so the development team could review the contents and create a necessary issues set in modules related.

### Pull Requests

Pull request submission is something more complicated because of the suite is highly modular and particular changes are required to be pulled in convenient repository.

Depending on what you want to change, proceed your pull request to following repositories:

* Algebra (curves and finite fields operations) (https://github.com/nilfoundation/crypto3-algebra.git)
* Block Ciphers (encrypt/decrypt with ```block``` namespace member policy) (https://github.com/nilfoundation/crypto3-block.git)
* Codecs (encoding/decoding) (https://github.com/nilfoundation/crypto3-codec.git)
* Hashes (https://github.com/nilfoundation/crypto3-hash.git)
* Key Derivation Functions (https://github.com/nilfoundation/crypto3-kdf.git)
* Message Authentication Codes (https://github.com/nilfoundation/crypto3-mac.git)
* Cipher Modes (encrypt/decrypt either with ```block``` or with ```stream``` namespace member policies) (https://github.com/nilfoundation/crypto3-modes.git)
* Multiprecision (https://github.com/nilfoundation/crypto3-multiprecision.git)
* Passhash (https://github.com/nilfoundation/crypto3-passhash.git)
* Public Key Schemes (`pubkey` namespace) (https://github.com/nilfoundation/crypto3-pubkey.git)
* Public Key Schemes Modes (threshold enhancements to public key schemes)(https://github.com/nilfoundation/crypto3-pubkey.git)
* Stream Ciphers (https://github.com/nilfoundation/crypto3-stream.git)
* Random (https://github.com/nilfoundation/crypto3-random.git)
* Verifiable Delay Functions (https://github.com/nilfoundation/crypto3-vdf.git)
* Zero-Knowledge Proofs (https://github.com/nilfoundation/crypto3-zk.git)

In case you are not sure which repository to proceed to, create an issue in this repository and ask.

## Community

This cryptography suite is authored by [=nil; Crypto3](https://crypto3.nil.foundation) team, so you can contact it
 several ways:
 * E-Mail. Just drop a line to [nemo@nil.foundation](mailto:nemo@nil.foundation).
 * Telegram Group. Join our Telegram group [@nilfoundation](https://t.me/nilfoundation) and ask any question in there.
 * Issue. Issue which does not belong to any particular module (or you just don't know where to put it) can be
  created in this repository. The team will answer that.
 * Discussion Topic (proposal, tutorial request, suggestion, etc). Would be happy to discuss that in the repository's GitHub Discussions: https://github.com/NilFoundation/crypto3/discussions
