\page BLS BLS weighted threshold signature

BLS weighted threshold signature
=============================

In this tutorial we dive deeper into the cryptography constructs which crypto3 library implements.
By the end of this tutorial , a user will understand how to sign data and use different modes.


Pre-requisites
================
The user has set up their development environment as outlined in the quickstart tutorial [TODO : here]() 
and can compile example programs in the scaffold repository.


Background
===============
ECDSA signatures are fairly common in most protocols ex Bitcoin/Ethereum. BLS signatures use 
different curve/computation which enables nicer properties such as signature aggregation. 
Signature aggregation allows for multiple parties to compute a single signature. ECDSA multi-sigs 
leak privacy as it is easy to identify a multi-sig public key and transactions leak data like the 
signer or scheme e.g: 3 of 5 signatures and needs to be done onchain for each signature. BLS signatures 
can be aggregated offline and 
appear no different from a traditional public key. Thus preserving privacy and reducing compute resources 
in verification. Weighted threshold signatures take this concept a step further by assigning weights
to each of the keys and only considering a signature valid when a threshold is reached.

The example we will construct is for a BLS weighted threshold scheme using the crypto3 library.


Weighted BLS Threshold Signatures
==========

The following pattern is used throughout the library.
- Initialization : This phase involves creating the cryptographic object. Ex: Key-Pair.
- Accumulation:  Accumulation is done (where needed) as a repeated set of operations involving the object on
which an algorithm needs to be performed. 
- Finalisation: This steps requires accumulated data to be finalised padded before running an algorithm.

Algorithms in the crypto3 library will carry out the above steps for the user. Algorithms can do operations
such as signing , aggregating , verifying.


Starting point to use any public key operations in crypto3 library is to create a cryptographic scheme. 

Below create a BLS scheme as follows. Currently supported curves are `BLS12-381` and `BLS12-377`
Schemes are also customisable and the following parameters.
//TODO
- `bls_default_public_params`
- `bls_mps_ro_version`
- `bls_basic_scheme`

```C++
using curve_type = nil::crypto3::algebra::curves::bls12_381;
using base_scheme_type = bls<bls_default_public_params<>, bls_mps_ro_version, bls_basic_scheme, curve_type>;
```

Schemes can operate in two [modes](https://github.com/NilFoundation/crypto3-pkmodes/). 
- Isomorphic : Single key 
- Threshold : Multiple key operations

Weighted threshold schemes can have one of the following secret sharing mechanisms.
- `Weighted Shamir Secret Sharing` 
- TODO Add others

Once the scheme is selected, we can pull the types out for the public/private keys.  

```c++
using mode_type = modes::threshold<base_scheme_type, weighted_shamir_sss>;
using scheme_type = typename mode_type::scheme_type;
using privkey_type = private_key<scheme_type>;
using pubkey_type = public_key<scheme_type>;
```


Next we set up some types which we will use throughout the example
- `sss_public_key_group_type` : TODO : Describe them. Which one of these are accumulators?
- `shares_dealing_processing_mode`
- `signing_processing_mode_type`
- `verification_processing_mode_type`
- `aggregation_processing_mode_type`

These types act as inputs to algorithms and are usually a parameter of the scheme/mode.

```c++
using sss_public_key_group_type = typename pubkey_type::sss_public_key_group_type;
using shares_dealing_processing_mode = typename nil::crypto3::pubkey::modes::isomorphic<sss_public_key_group_type>::template bind<
        nil::crypto3::pubkey::shares_dealing_policy<sss_public_key_group_type>>::type;
using signing_processing_mode_type = typename mode_type::template bind<typename mode_type::signing_policy>::type;
using verification_processing_mode_type =
        typename mode_type::template bind<typename mode_type::verification_policy>::type;
using aggregation_processing_mode_type =
        typename mode_type::template bind<typename mode_type::aggregation_policy>::type;
```

We now take a closer look at our main.

- `n` : Is the number of keys we want our `dealer` to create
- `t` : Is the threshold value which must be met for a signature to be considered valid 

```c++
    std::size_t n = 20;
    std::size_t t = 10;
```
Next , we assign weights to our threshold values using the `weights_type` type which is a pair of (TODO)

```c++
    auto i = 1;
    auto j = 1;
    typename privkey_type::weights_type weights;
    std::generate_n(std::inserter(weights, weights.end()), n, [&i, &j, &t]() {
        j = j >= t ? 1 : j;
        return std::make_pair(i++, j++);
    });
```

The `dealer` next creates keys adhering to these parameters.

```c++
    auto coeffs = sss_public_key_group_type::get_poly(t, n);
    auto [PK, privkeys] = nil::crypto3::create_key<scheme_type>(coeffs, n, weights);
```


We now sign this data using **all** private keys we see two algorithms
- `sign` : The algorithm takes the parameters of scheme type, the message and the signing processing mode.
- `part_verify`. This is an intermediate step before signature aggregation to verify. (TODO check priv key)

```c++
    std::vector<typename privkey_type::part_signature_type> part_signatures;
    for (auto &sk : privkeys) {
        part_signatures.emplace_back(
                nil::crypto3::sign<scheme_type, decltype(msg), signing_processing_mode_type>(msg, weights, sk));
        BOOST_CHECK(static_cast<bool>(
                            nil::crypto3::part_verify<mode_type>(msg.begin(), msg.end(), part_signatures.back(), weights, sk)));
    }
```

Next we run the following algorithms
- `aggregate` : Algorithm aggregates all signatures into a single one. 
- `verify` : The verification algorithm validates the signature against the message.

```c++
    typename pubkey_type::signature_type sig =
            nil::crypto3::aggregate<scheme_type, decltype(std::cbegin(part_signatures)), aggregation_processing_mode_type>(
                    std::cbegin(part_signatures), std::cend(part_signatures));
    BOOST_CHECK(static_cast<bool>(
                        nil::crypto3::verify<scheme_type, decltype(msg), verification_processing_mode_type>(msg, sig, PK)));
```


Next we do a threshold signing where only signatories until the threshold is reached participate in the signing.

```c++
    std::vector<typename privkey_type::part_signature_type> part_signatures_t;
    typename privkey_type::weights_type confirmed_weights;
    std::vector<privkey_type> confirmed_keys;
    auto weighted_keys_it = std::cbegin(privkeys);
    auto weight = 0;
    while (weight < t) {
        confirmed_keys.emplace_back(*weighted_keys_it);
        confirmed_weights.emplace(weighted_keys_it->get_index(), weights.at(weighted_keys_it->get_index()));
        weight += weighted_keys_it->get_weight();
        ++weighted_keys_it;
    }

    for (auto &sk : confirmed_keys) {
        part_signatures_t.emplace_back(
                nil::crypto3::sign<scheme_type, decltype(msg), signing_processing_mode_type>(msg, confirmed_weights, sk));
        BOOST_CHECK(static_cast<bool>(nil::crypto3::part_verify<mode_type>(
                msg.begin(), msg.end(), part_signatures_t.back(), confirmed_weights, sk)));
    }
```

Same as previously , we aggregate and verify these subset of signatures

```c++
    typename pubkey_type::signature_type sig_t =
            nil::crypto3::aggregate<scheme_type, decltype(std::cbegin(part_signatures_t)),
                    aggregation_processing_mode_type>(std::cbegin(part_signatures_t),
                                                      std::cend(part_signatures_t));
    BOOST_CHECK(static_cast<bool>(
                        nil::crypto3::verify<scheme_type, decltype(msg), verification_processing_mode_type>(msg, sig_t, PK)));
```

