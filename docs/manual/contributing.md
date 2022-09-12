### Contributing


We welcome contribution, bug reports, fixes and suggestions from the community.

### Branching guide

These branches are partially compliant to GitFlow branching model:
* `master` - contains tagged release versions.
* `develop` - contains in-development library state, used for feature-branching.
* `#version-branch` - contains changes related to particular version development.
* `#issue-branch` - contains changes related to particular issue.

The above nomenclature is followed for both umbrella-repository and single module repositories.

### Issues

Preferred way to submit an issue is to submit that to the particular module you
think the trouble is in. In case you are not sure, submit an issue to this (master)
repository, so the development team could review the contents and create a necessary
issues set in modules related.

### Creating a discussion
Discussions should be created on GitHub for any proposed changes or suggestions to the library 


### Pull Requests

Pull request submission is complicated because of the suite is highly modular and particular changes are required to be pulled in convenient repository.

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
