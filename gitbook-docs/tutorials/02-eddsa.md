# EdDSA signature verification

EdDSA signature plays a significant role in the blockchain world.
In the zkBridge tutorial serie, we will use it to verify validators' signatures.

In this tutorial, we will show you how to implement the 
EdDSA signature verification algorithm in the circuit form.

EdDSA signature is based on ED25519 elliptic curve and its Galois field.
So, the following SDK headers need to be included to support 
circuit-friendly implementations of curves and fields arithmetic:

```cpp
#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>
```

Now we have types for the curve and field elements:

* `typename ed25519::template g1_type<>::value_type` — g1 group element,
* `typename ed25519::base_field_type::value_type` — ed25519 curve base field type,
* `typename ed25519::scalar_field_type::value_type` — ed25519 curve scalar field type.

All these three types are available from `nil::crypto3::algebra::curves` namespace,
so we are going to use it instead of writing `nil::crypto3::algebra::curves::ed25519` every time:

```cpp
#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>

using namespace nil::crypto3::algebra::curves;
```

The EdDSA signature result consists of two elements: `R` and `s`.
`R` is an element of the g1 group, and `s` is an element of the scalar field.
We will use a `struct` to represent the signature result:

```cpp
#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>

using namespace nil::crypto3::algebra::curves;

struct eddsa_signature_type {
    typename ed25519::template g1_type<>::value_type R;
    typename ed25519::scalar_field_type::value_type s;
};
```

We also need a type for the message, which is going to be signed.
At the moment of writing there are some limitations on the message size,
so we will use a fixed size array of the Pallas field elements:

```cpp
#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(4)))
                typename pallas::base_field_type::value_type eddsa_message_block_type;

struct eddsa_signature_type {
    typename ed25519::template g1_type<>::value_type R;
    typename ed25519::scalar_field_type::value_type s;
};
```

Now we can define the signature verification function:

```cpp
#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

typedef __attribute__((ext_vector_type(4)))
                typename pallas::base_field_type::value_type eddsa_message_block_type;

struct eddsa_signature_type {
    typename ed25519::template g1_type<>::value_type R;
    typename ed25519::scalar_field_type::value_type s;
};

[[circuit]] bool verify_eddsa_signature (eddsa_signature_type input, 
    typename ed25519::template g1_type<>::value_type pk,
    eddsa_message_block_type M) {

        typename ed25519::template g1_type<>::value_type B =
                    ed25519::template g1_type<>::value_type::one();
        __zkllvm_field_curve25519_scalar k = 
                    __builtin_assigner_sha2_512_curve25519(input.R.data, pk.data, M);

        return (B*input.s - (input.R + (pk*k))).is_zero();
    }
```

{% hint style="info" %}
You've learned how to implement the EdDSA signature verification algorithm in the circuit form.
Now you can directly take it from Crypto3 library and use it in your circuits.
{% endhint %}

# Using the implementation of EdDSA signature verification from Crypto3 library

```cpp
#include <nil/crypto3/pubkey/eddsa.hpp>

typedef __attribute__((ext_vector_type(4)))
                typename pallas::base_field_type::value_type eddsa_message_block_type;

[[circuit]] int main (){

    eddsa_message_block_type msg = {0, 1, 2, 3};

    public_key<eddsa> pk;
    typename eddsa::signature_type sig;

    verify(msg, sig, pubkey);
    return 0;
}
```