# <span style='font-family: monospace'>=nil;</span> Crypto3 Cryptography Suite (libcrypto3) {#mainpage}

Crypto3 cryptography suite intention is:
1. To provide a secure, fast and architecturally clean C++ generic cryptography schemes implementation.
2. To provide a developer-friendly, modular suite, usable for novel schemes implementation and further
 extension.
3. To provide a Standard Template Library-alike C++ interface and concept-based architecture implementation.

## Contents

Cryptography suite contains multiple modules from
encoding (e.g. base64) to complex public-key
protocols (e.g. threshold schemes, zero-knowledge protocol or verifiable delay functions).

Detailed list of modules available is accessible at
[Modules page](modules.html).

## Usage

Cryptography suite comes in several modifications:

1. Generic usage.
2. In-Boost usage. 
3. Particular module usage.
4. Usage via other language bindings.

### Generic Usage 
Generic usage-intended cryptography suite is split to modules available at [GitHub](https://github.com/nilfoundation). 
Some of them are not for standalone usage, some of them are. So there is a suite superproject repository available at
 [Crypto3 GitHub](https://github.com/nilfoundation/crypto3.git). Superproject includes all the modules
available as submodules emplaced in `libs` directory.

This case is handled by several branches of the same
intention in all the module repositories as well as in
superproject repository. These branches are partially compliant to GitFlow branching model:
* `master` - contains tagged release versions.
* `develop` - contains in-development library state, used for feature-branching.
* `#version-branch` - contains changes related to particular version development. 
* `#issue-branch` - contains changes related to particular issue.

Such a case supposes all the actual development is
processed in modules repositories, so there would 
not be many of issue-related branches in superproject
repository.

Generic-purposed suite uses CMake as build system with
[cmake-modules](https://github.com/BoostCMake/cmake-modules.git) CMake library used for easier
in-project targets management. 
  
### In-Boost Usage

In-Boost usage supposes the library to contain only
proven and secure state-of-art modules packed into
one repository with no submodules at all. 

For now this gets accomplished by several branches:
* `boost-master` - contains tagged release version of Boost-ified library with modules only approved by
 Boost community to be included in library. 
* `boost-develop` - contains in-development library state.

All the actual development is processed in library's 
generic-purpose modification, so this means 
Boost-related branches are merge-only.

Later these branches will be factored out to separate Boost-related repository. 

### Particular Module Usage

Usage of particular modules is supposed to be useful for minimization of resulting project sources amount
and reducing the dependencies tree height.
