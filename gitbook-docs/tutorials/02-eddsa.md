# EDDSA signature verification

EDDSA signature plays a big role in the blockchain world. It is used in many places, but in the terms of the zk-Bridge tutorial serie we need it for validators signatures verification.

In this tutorial we will show you how to implement EDDSA signature verification algorithm in the circuit form.

EDDSA signature is based on ED25519 elliptic curve and its Galois field. So, following SDK headers need to be included in order to support circuit-friendly the curve and field arithmetic:

```cpp
#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>
```

Now we have types for the curve and field elements: `typename ed25519::template g1_type<>::value_type` - g1 group element, `typename ed25519::base_field_type::value_type` - ed25519 curve base field type and `typename ed25519::scalar_field_type::value_type` - ed25519 curve scalar field type. All these three types are avialable from `nil::crypto3::algebra::curves` namespace, so we are going to use it instead of writing `nil::crypto3::algebra::curves::ed25519` every time.

```cpp
#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>

using namespace nil::crypto3::algebra::curves;
```

EDDSA signature result consist of two elements: `R` and `s`. `R` is element of g1 group, and `s` is the element of scalar field. We will use a struct to represent the signature result:

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

We also need a type for the message (the message which is going to be signed). Right now we have some liimitations on the message size, so we will use a fixed size array of pallas field elements:

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

        typename ed25519::template g1_type<>::value_type B = ed25519::template g1_type<>::value_type::one();
        __zkllvm_field_curve25519_scalar k = __builtin_assigner_sha2_512_curve25519(input.R.data, pk.data, M);

        return (B*input.s - (input.R + (pk*k))).is_zero();
    }
```