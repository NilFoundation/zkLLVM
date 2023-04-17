---
description: Setting up zkLLVM
---

# Environment Setup

In this guide, we will set up the dependencies required for compiling zkLLVM locally.

{% hint style="info" %}
zkLLVM needs to be compiled locally at the moment; we will be publishing binaries via package managers soon.
{% endhint %}

## Installation

### Linux

The following dependencies need to be installed.

* [boost](https://www.boost.org/) >= 1.74.0
* [cmake](https://cmake.org/) >= 3.5
* [clang](https://clang.llvm.org/) >= 14.0.6

Please execute the below to fetch the packages required or adapt the command to your package manager.

```shell
sudo apt install build-essential libssl-dev libboost-all-dev cmake clang git
```

Once the base packages are installed, please see the installation on how you can build the zkLLVM artefacts.
