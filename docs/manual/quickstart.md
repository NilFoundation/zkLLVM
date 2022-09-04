\page Quickstart Quickstart

Quickstart
========================

By the end of this guide ,you will have set up a development environment for crypto3 projects 
and be able to run an example.This will enable you to test ideas quickly and further explore the 
API's of the suite.

## Install dependencies

### Linux

The following dependencies need to be installed.

- [Boost](https://www.boost.org/) >= 1.74.0
- [cmake](https://cmake.org/) >= 3.5
- [clang](https://clang.llvm.org/) >= 14.0.6

Please execute the below to fetch the packages required or adapt the command
to your package manager.

```sudo apt install build-essential libssl-dev libboost-all-dev cmake clang git```


## Get crypto3 scaffold project

```
git clone https://github.com/NilFoundation/crypto3-scaffold.git
cd crypto3-scaffold
```

## Project structure

The project is an example of generic usage of the suite,adding the whole crypto3 suite as a submodule 
dependency.

```
root
├── libs : submodule including the mono-repository for crypto3 suite
├── src  
│   ├── bls 
│   │  │──── src: source for bls signing example.

```

## Installing and testing scaffold

- **Clone submodules recursively**
```
git submodule update --init --recursive
```

- **Build** : The project is built using cmake system.

``` 
mkdir build && cd build
cmake .. && make
```

- Run executable
``` 
./src/bls/crypto3-scaffold
```

You should see the output `Verified signature successfully` on your console.

## Conclusion
Congratulations! You now have the environment to start experimenting with the crypto3 suite. You can now explore
other [modules](modules.html) in the suite.Modules also have examples in their repositories' ex : [algebra example](https://github.com/NilFoundation/crypto3-algebra/tree/master/example).


## Common Issues
### Compilation Errors
If you have more than one compiler installed i.e g++ & clang++. The make system might pick up the former. You can explicitly force usage of
clang++ by finding the path and passing it in the variable below.

```
`which clang++`  
cmake .. -DCMAKE_CXX_COMPILER=<path to clang++ from above>
```

### Submodule management
Git maintains a few places where submodule details are cached. Sometimes updates do not come through. ex: deletion , updating
a previously checked out submodule.It is advisable to check these locations for remains or try a new checkout.
- .gitmodules
- .git/config
- .git/modules/*
