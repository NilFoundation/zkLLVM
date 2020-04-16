# =nil; Foundation's Cryptography Suite (libcrypto3)
[![Build Status](https://travis-ci.com/NilFoundation/crypto3.svg?branch=master)](https://travis-ci.com/NilFoundation/crypto3)[![Coverage Status](https://coveralls.io/repos/github/NilFoundation/crypto3/badge.svg?branch=master)](https://coveralls.io/github/NilFoundation/crypto3?branch=master)

Modern cryptography suite built in C++ with concept-based architecture.

Development repository is https://github.com/nilfoundation/crypto3.
 
Rationale, detailed tutorials and references are available at https://crypto3.nil.foundation/projects/crypto3.
 
Developed by =nil; Crypto3 and supported by =nil; Foundation: https://nil.foundation.

All the communication is being processed via GitHub issues.

This repository is a master-repository for the whole suite. Particular-purposed libraries repositories (e.g. [block](https://github.com/nilfoundation/block) or [hash](https://github.com/nilfoundation/hash) are not intended to be used outside the suite and should be handled with great care.

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
     
Fine example of such embedding is =nil; Foundation's Multithreading Suite 
(https://github.com/nilfoundation/mtl). It uses only 
[hashes](https://github.com/nilfoundation/hash) so the dependency graph requires 
for the project to submodule [block ciphers library](https://github.com/nilfoundation/block) and optional 
[codec library](https://github.com/nilfoundation/codec) for testing purposes. So, 
the root MTL repository has only related libraries submoduled: 
[block](https://github.com/nilfoundation/mtl/libs/block), 
[codec](https://github.com/nilfoundation/mtl/libs/codec) and 
[hash](https://github.com/nilfoundation/mtl/hash).
        
## Repository Contents

This particular repository contains all of the modules available within the suite as submodules emplaced in
 ```libs``` directory. This is called "Modularized Version". 

### Modularized Version

Modularized version contains all of the modules available emplaced in ```libs``` directory:

* Block Ciphers (https://github.com/nilfoundation/block.git)
* Codecs (https://github.com/nilfoundation/codec.git)
* Hashes (https://github.com/nilfoundation/hash.git)
* Key Derivation Functions (https://github.com/nilfoundation/kdf.git)
* Message Authentication Codes (https://github.com/nilfoundation/mac.git)
* Cipher Modes (https://github.com/nilfoundation/modes.git)
* Multiprecision (https://github.com/nilfoundation/multiprecision.git)
* Stream Ciphers (https://github.com/nilfoundation/stream.git)
* Verifiable Delay Functions (https://github.com/nilfoundation/vdf.git)

## Contributing

### Issues

Preferred way to submit an issue is to submit that to the particular module you think the trouble is in. In case you are not sure, submit an issue to this (master) repository, so the development team could review the contents and create a necessary issues set in modules related.

### Pull Requests

Pull request submission is something more complicated because of the suite is highly modular and particular changes are required to be pulled in convenient repository.

Depending on what you want to change, proceed your pull request to following repositories:

* Block Ciphers (encrypt/decrypt with ```block``` namespace member policy) (https://github.com/nilfoundation/block.git)
* Codecs (encoding/decoding) (https://github.com/nilfoundation/codec.git)
* Hashes (hashing) (https://github.com/nilfoundation/hash.git)
* Key Derivation Functions (derivation) (https://github.com/nilfoundation/kdf.git)
* Message Authentication Codes (compute/verify) (https://github.com/nilfoundation/mac.git)
* Cipher Modes (encrypt/decrypt either with ```block``` or with ```stream``` namespace member policies) (https://github.com/nilfoundation/modes.git)
* Stream Ciphers (encrypt/decrypt with ```stream``` namespace member policy)  (https://github.com/nilfoundation/stream.git)

In case you are not sure which repository to proceed to, create an issue in this repository and ask.
