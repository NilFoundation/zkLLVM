# Nil Foundation Cryptography Library (libcrypto3)
[![Build Status](https://travis-ci.com/NilFoundation/crypto3.svg?branch=master)](https://travis-ci.com/NilFoundation/crypto3)[![Coverage Status](https://coveralls.io/repos/github/NilFoundation/crypto3/badge.svg?branch=master)](https://coveralls.io/github/NilFoundation/crypto3?branch=master)

Modern cryptography backend built in C++ with concept-based architecture.

Development repository is https://github.com/nilfoundation/crypto3.
 
Rationale, detailed tutorials and references are available at https://crypto3.nil.foundation.
 
Developed by =nil; Crypto3 and supported by =nil; Foundation: https://nil.foundation.

All the communication is being processed via GitHub issues.

This repository is a master-repository for the whole suite. Particular-purposed libraries repositories (e.g. ```block``` or  ```hash```) are not intended to be used outside the suite and should be handled with great care.

## Repository Structure

This repository structure varies depending on particular usage purpose:

1. Generic cryptography suite usage.

    Useful for submoduling with projects with CMake build system or for usual build and install routine.

    This case gets covered with submodule-based header-only superproject including all the stable libraries available
    . Every library is emplaced in ```libs/%library_name%``` as submodule. Depends on Boost (https://boost.org). Uses
     CMake build system. 

    Handled by branches ```master```, ```develop``` and ```#issue-name``` branches (uses git-flow) along with semantic versioning-tagged
 ```master``` branch commits (e.g. ```1.2.34```)  

2. In-Boost (https://boost.org) usage.

    Useful for usage with tight Boost library suite integration, represents a Boost-ified version of a
     library.
     
     This case gets covered by header-only library with Boost-community approved libraries set.
     
     This is handled by branches ```boost-master```, ```boost-develop``` and ```#issues-boost``` branches. Uses CMake and Bjam build systems. 
     
3. Custom libraries set usage. 

    Suite is designed in a way for the every particular user could include and exclude
 particular libraries and features depending on a usecase.
 
    This could be useful for keeping a codebase small and tiny along with keeping submodules tree height <= 1.
 
    This could be covered by couple of ways:
    
    1. Taking a look at dependency graph of each library (included in every library repository description) and
     embedding the particular set of libraries in the user's project.
     
        Fine example of such embedding is =nil; Foundation's Multithreading Library (https://github.com/nilfoundation/mtl). 
        It uses only hashes (https://github.com/nilfoundation/hash) so the dependency graph requires for the project
         to submodule block ciphers library (https://github.com/nilfoundation/block) and optional codec libray for
          testing
          purposes (https://github.com/nilfoundation/codec). So, the root MTL repository has only related libraries
           submoduled: block (https://github.com/nilfoundation/mtl/libs/block), codec (https://github.com
           /nilfoundation/mtl/libs/codec) and hash (https://github.com/nilfoundation/mtl/hash).
