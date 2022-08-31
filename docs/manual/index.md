# <span style='font-family: monospace'>=nil;</span> Crypto3 C++ Cryptography Suite {#mainpage}

@tableofcontents

Crypto3 cryptography suite purpose is:
1. To provide a secure, fast and architecturally clean C++ generic cryptography schemes implementation.
2. To provide a developer-friendly, modular suite, usable for novel schemes implementation and further
 extension.
3. To provide a Standard Template Library-alike C++ interface and concept-based architecture implementation.

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
Some of them are not for standalone usage, some of them are. So there is a suite super-project repository available at
 [Crypto3 GitHub](https://github.com/nilfoundation/crypto3.git). Super-project includes all the modules
available as submodules emplaced in `libs` directory. A developer can thus include the super-project as a submodule in their 
project and would not need to resolve dependencies of each module. See [crypto3-scaffold](https://github.com/NilFoundation/crypto3-scaffold) as an example of usage. 

Generic-purposed suite uses CMake as build system with
[cmake-modules](https://github.com/BoostCMake/cmake-modules.git) CMake library used for easier
in-project targets management.
  
### Selective module usage

Developer can select to include a one or more modules to reduce the sources of resulting project and dependencies tree height. This however
does require the developer to manually resolve all required dependencies.

### Branching/contributing 

These branches are partially compliant to GitFlow branching model:
* `master` - contains tagged release versions.
* `develop` - contains in-development library state, used for feature-branching.
* `#version-branch` - contains changes related to particular version development.
* `#issue-branch` - contains changes related to particular issue.

The above nomenclature is followed for both super-repository and single module repositories as well.