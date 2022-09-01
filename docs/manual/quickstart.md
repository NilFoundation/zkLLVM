## Quickstart

By the end of this guide , a developer will have set up a scaffold development environment and be able to run an example. 
This will enable developers to test ideas quickly and explore the API's of the suite.

## Install dependencies

### Linux

The following dependencies need to be installed.

- [Boost](https://www.boost.org/) >= 1.74.0
- [cmake](https://cmake.org/) >= 3.5
- [clang](https://clang.llvm.org/) >= 14.0.6

Please execute the below to fetch the packages required on Ubuntu LTS. Or adapt the command
to your package manager.
`sudo apt install build-essential libssl-dev libboost-all-dev cmake clang git`


### Windows
Coming soon!

### macOS
Coming soon!

## Get crypto3 scaffold project

```
git clone https://github.com/hgedia/crypto3-scaffold.git
cd crypto3-scaffold
```

## Crypto3 scaffold project structure

<pre>
root
├── packages
│   ├── 
│   ├── 
│   ├── 
│   ├── 
│   ├── 
│   ├──

</pre>
## Installing and testing scaffold

Clone all submodules recursively
```
git submodule update --init --recursive
```

Build
``` 
mkdir build && cd build
cmake .. && make
```

Run executable
``` 
./src/bls/crypto3-scaffold
```

User should see the output `verified signature successfully`


## Common Issues



## Conclusion