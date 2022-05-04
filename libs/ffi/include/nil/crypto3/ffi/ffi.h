#ifndef CRYPTO3_FFI_HPP
#define CRYPTO3_FFI_HPP

#ifdef __cplusplus
extern "C" {
#endif

/*
This header exports some of library functionality via a C89
interface. This API is uesd by the Python and OCaml bindings via those
languages respective ctypes libraries.

The API is intended to be as easy as possible to call from other
languages, which often have easy ways to call C, because C. But some C
code is easier to deal with than others, so to make things easy this
API follows a few simple rules:

- All interactions are via pointers to opaque structs. No need to worry about
  structure padding issues and the like.

- All functions return an int error code (except the version calls, which are
  assumed to always have something to say).

- Use simple types: size_t for lengths, const char* NULL terminated strings,
  uint8_t for binary.

- No ownership of memory transfers across the API boundary. The API will
  consume data from const pointers, and will produce output by writing to
  variables provided by the caller.

- If exporting a value (a string or a blob) the function takes a pointer to the
  output array and a read/write pointer to the length. If the length is insufficient, an
  error is returned. So passing nullptr/0 allows querying the final value.

  Note this does not apply to all functions, like `nil_crypto_hash_final`
  which is not idempotent and are documented specially. But it's a
  general theory of operation.

The API is not currently documented, nor should it be considered
stable. It is buggy as heck, most likely, and error handling is a
mess. However the goal is to provide a long term API usable for
language bindings, or for use by systems written in C. Suggestions on
how to provide the cleanest API for such users would be most welcome.

* TODO:
* - Better error reporting
* - User callback for exception logging?
* - Doxygen comments for all functions/params
*/

#include <stdint.h>
#include <stddef.h>

/**
 * Error codes
 */
enum CRYPTO3_FFI_ERROR {
    CRYPTO3_FFI_SUCCESS = 0,
    CRYPTO3_FFI_INVALID_VERIFIER = 1,

    CRYPTO3_FFI_ERROR_INVALID_INPUT = -1,
    CRYPTO3_FFI_ERROR_BAD_MAC = -2,

    CRYPTO3_FFI_ERROR_INSUFFICIENT_BUFFER_SPACE = -10,
    CRYPTO3_FFI_ERROR_EXCEPTION_THROWN = -20,
    CRYPTO3_FFI_ERROR_BAD_FLAG = -30,
    CRYPTO3_FFI_ERROR_NULL_POINTER = -31,
    CRYPTO3_FFI_ERROR_BAD_PARAMETER = -32,
    CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED = -40,
    CRYPTO3_FFI_ERROR_INVALID_OBJECT = -50,

    CRYPTO3_FFI_ERROR_UNKNOWN_ERROR = -100,
};

/**
 * Convert an error code into a string. Returns "Unknown error"
 * if the error code is not a known one.
 */
const char *nil_crypto_error_description(int err);

/**
 * Return the version of the currently supported FFI API. This is
 * expressed in the form YYYYMMDD of the release date of this version
 * of the API.
 */

uint32_t nil_crypto_ffi_api_version();

/**
 * Return 0 (ok) if the version given is one this library supports.
 * nil_crypto_ffi_supports_api(nil_crypto_ffi_api_version()) will always return 0.
 */

int nil_crypto_ffi_supports_api(uint32_t api_version);

/**
 * Return a free-form version string, e.g., 2.0.0
 */

const char *nil_crypto_version_string();

/**
 * Return the major version of the library
 */

uint32_t nil_crypto_version_major();

/**
 * Return the minor version of the library
 */

uint32_t nil_crypto_version_minor();

/**
 * Return the patch version of the library
 */

uint32_t nil_crypto_version_patch();

/**
 * Return the date this version was released as
 * an integer, or 0 if an unreleased version
 */

uint32_t nil_crypto_version_datestamp();

#define CRYPTO3_FFI_HEX_LOWER_CASE 1

/**
 * Perform hex encoding
 * @param x is some binary data
 * @param len length of x in bytes
 * @param out an array of at least x*2 bytes
 * @param flags flags out be upper or lower case?
 * @return 0 on success, 1 on failure
 */

int nil_crypto_hex_encode(const uint8_t *x, size_t len, char *out, uint32_t flags);

/**
 * Perform hex decoding
 * @param hex_str a string of hex chars (whitespace is ignored)
 * @param in_len the length of hex_str
 * @param out the output buffer should be at least strlen(hex_str)/2 bytes
 * @param out_len the size of out
 */

int nil_crypto_hex_decode(const char *hex_str, size_t in_len, uint8_t *out, size_t *out_len);

/**
 * Perform base64 encoding
 */

int nil_crypto_base64_encode(const uint8_t *x, size_t len, char *out, size_t *out_len);

/**
 * Perform base64 decoding
 */

int nil_crypto_base64_decode(const char *base64_str, size_t in_len, uint8_t *out, size_t *out_len);

/**
 * RNG type
 */
typedef struct nil_crypto_rng_struct *nil_crypto_rng_t;

/**
 * Initialize a random number generator object
 * @param rng rng object
 * @param rng_type type of the rng, possible values:
 *    "system": System_RNG, "user": AutoSeeded_RNG
 * Set rng_type to null or empty string to let the library choose
 *
 * TODO: replace rng_type with simple flags?
 */

int nil_crypto_rng_init(nil_crypto_rng_t *rng, const char *rng_type);

/**
 * Get random bytes from a random number generator
 * @param rng rng object
 * @param out output buffer of size out_len
 * @param out_len number of requested bytes
 * @return 0 on success, negative on failure
 *
 * TODO: better name
 */

int nil_crypto_rng_get(nil_crypto_rng_t rng, uint8_t *out, size_t out_len);

/**
 * Reseed a random number generator
 * Uses the System_RNG as a seed generator.
 *
 * @param rng rng object
 * @param bits number of bits to to reseed with
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_rng_reseed(nil_crypto_rng_t rng, size_t bits);

/**
 * Frees all resources of the random number generator object
 * @param rng rng object
 * @return always returns 0
 */

int nil_crypto_rng_destroy(nil_crypto_rng_t rng);

/*
 * Hash type
 */
typedef struct nil_crypto_hash_struct *nil_crypto_hash_t;

/**
 * Initialize a hash function object
 * @param hash hash object
 * @param hash_name name of the hash function, e.g., "SHA-384"
 * @param flags should be 0 in current API revision, all other uses are reserved
 *       and return CRYPTO3_FFI_ERROR_BAD_FLAG
 *
 * TODO: since output_length is effectively required to use this API,
 * return it from init as an output parameter
 */

int nil_crypto_hash_init(nil_crypto_hash_t *hash, const char *hash_name, uint32_t flags);

/**
 * Copy the state of a hash function object
 * @param dest destination hash object
 * @param source source hash object
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_hash_copy_state(nil_crypto_hash_t *dest, const nil_crypto_hash_t source);

/**
 * Writes the output length of the hash function to *output_length
 * @param hash hash object
 * @param output_length output buffer to hold the hash function output length
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_hash_output_length(nil_crypto_hash_t hash, size_t *output_length);

/**
 * Writes the block size of the hash function to *block_size
 * @param hash hash object
 * @param block_size output buffer to hold the hash function output length
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_hash_block_size(nil_crypto_hash_t hash, size_t *block_size);

/**
 * Send more input to the hash function
 * @param hash hash object
 * @param in input buffer
 * @param in_len number of bytes to read from the input buffer
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_hash_update(nil_crypto_hash_t hash, const uint8_t *in, size_t in_len);

/**
 * Finalizes the hash computation and writes the output to
 * out[0:nil_crypto_hash_output_length()] then reinitializes for computing
 * another digest as if nil_crypto_hash_clear had been called.
 * @param hash hash object
 * @param out output buffer
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_hash_final(nil_crypto_hash_t hash, uint8_t out[]);

/**
 * Reinitializes the state of the hash computation. A hash can
 * be computed (with update/final) immediately.
 * @param hash hash object
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_hash_clear(nil_crypto_hash_t hash);

/**
 * Frees all resources of the hash object
 * @param hash hash object
 * @return always returns 0
 */

int nil_crypto_hash_destroy(nil_crypto_hash_t hash);

/**
 * TODO has no implementation
 */

int nil_crypto_hash_name(nil_crypto_hash_t hash, char *name, size_t name_len);

/*
 * Message Authentication type
 */
typedef struct nil_crypto_mac_struct *nil_crypto_mac_t;

/**
 * Initialize a message authentication code object
 * @param mac mac object
 * @param mac_name name of the hash function, e.g., "HMAC(SHA-384)"
 * @param flags should be 0 in current API revision, all other uses are reserved
 *       and return a negative value (error code)
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_mac_init(nil_crypto_mac_t *mac, const char *mac_name, uint32_t flags);

/**
 * Writes the output length of the message authentication code to *output_length
 * @param mac mac object
 * @param output_length output buffer to hold the MAC output length
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_mac_output_length(nil_crypto_mac_t mac, size_t *output_length);

/**
 * Sets the key on the MAC
 * @param mac mac object
 * @param key buffer holding the key
 * @param key_len size of the key buffer in bytes
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_mac_set_key(nil_crypto_mac_t mac, const uint8_t *key, size_t key_len);

/**
 * Send more input to the message authentication code
 * @param mac mac object
 * @param buf input buffer
 * @param len number of bytes to read from the input buffer
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_mac_update(nil_crypto_mac_t mac, const uint8_t *buf, size_t len);

/**
 * Finalizes the MAC computation and writes the output to
 * out[0:nil_crypto_mac_output_length()] then reinitializes for computing
 * another MAC as if nil_crypto_mac_clear had been called.
 * @param mac mac object
 * @param out output buffer
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_mac_final(nil_crypto_mac_t mac, uint8_t out[]);

/**
 * Reinitializes the state of the MAC computation. A MAC can
 * be computed (with update/final) immediately.
 * @param mac mac object
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_mac_clear(nil_crypto_mac_t mac);

/**
 * Frees all resources of the MAC object
 * @param mac mac object
 * @return always returns 0
 */

int nil_crypto_mac_destroy(nil_crypto_mac_t mac);

/*
 * Cipher modes
 */
typedef struct nil_crypto_cipher_struct *nil_crypto_cipher_t;

#define CRYPTO3_CIPHER_INIT_FLAG_MASK_DIRECTION 1
#define CRYPTO3_CIPHER_INIT_FLAG_ENCRYPT 0
#define CRYPTO3_CIPHER_INIT_FLAG_DECRYPT 1

int nil_crypto_cipher_init(nil_crypto_cipher_t *cipher, const char *name, uint32_t flags);

int nil_crypto_cipher_valid_nonce_length(nil_crypto_cipher_t cipher, size_t nl);

int nil_crypto_cipher_get_tag_length(nil_crypto_cipher_t cipher, size_t *tag_size);

int nil_crypto_cipher_get_default_nonce_length(nil_crypto_cipher_t cipher, size_t *nl);

int nil_crypto_cipher_get_update_granularity(nil_crypto_cipher_t cipher, size_t *ug);

int nil_crypto_cipher_query_keylen(nil_crypto_cipher_t, size_t *out_minimum_keylength, size_t *out_maximum_keylength);

int nil_crypto_cipher_set_key(nil_crypto_cipher_t cipher, const uint8_t *key, size_t key_len);

int nil_crypto_cipher_set_associated_data(nil_crypto_cipher_t cipher, const uint8_t *ad, size_t ad_len);

int nil_crypto_cipher_start(nil_crypto_cipher_t cipher, const uint8_t *nonce, size_t nonce_len);

#define CRYPTO3_CIPHER_UPDATE_FLAG_FINAL (1U << 0)

int nil_crypto_cipher_update(nil_crypto_cipher_t cipher, uint32_t flags, uint8_t output[], size_t output_size,
                             size_t *output_written, const uint8_t input_bytes[], size_t input_size,
                             size_t *input_consumed);

int nil_crypto_cipher_clear(nil_crypto_cipher_t hash);

int nil_crypto_cipher_destroy(nil_crypto_cipher_t cipher);

/*
 * Derive a key from a passphrase for a number of iterations
 * @param pbkdf_algo PBKDF algorithm, e.g., "PBKDF2"
 * @param out buffer to store the derived key, must be of out_len bytes
 * @param out_len the desired length of the key to produce
 * @param passphrase the password to derive the key from
 * @param salt a randomly chosen salt
 * @param salt_len length of salt in bytes
 * @param iterations the number of iterations to use (use 10K or more)
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_pbkdf(const char *pbkdf_algo, uint8_t out[], size_t out_len, const char *passphrase,
                     const uint8_t salt[], size_t salt_len, size_t iterations);

/**
 * Derive a key from a passphrase, running until msec time has elapsed.
 * @param pbkdf_algo PBKDF algorithm, e.g., "PBKDF2"
 * @param out buffer to store the derived key, must be of out_len bytes
 * @param out_len the desired length of the key to produce
 * @param passphrase the password to derive the key from
 * @param salt a randomly chosen salt
 * @param salt_len length of salt in bytes
 * @param milliseconds_to_run if iterations is zero, then instead the PBKDF is
 *        run until milliseconds_to_run milliseconds has passed
 * @param out_iterations_used set to the number iterations executed
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_pbkdf_timed(const char *pbkdf_algo, uint8_t out[], size_t out_len, const char *passphrase,
                           const uint8_t salt[], size_t salt_len, size_t milliseconds_to_run,
                           size_t *out_iterations_used);

/**
 * Derive a key
 * @param kdf_algo KDF algorithm, e.g., "SP800-56C"
 * @param out buffer holding the derived key, must be of length out_len
 * @param out_len the desired output length in bytes
 * @param secret the secret input
 * @param secret_len size of secret in bytes
 * @param salt a diversifier
 * @param salt_len size of salt in bytes
 * @param label purpose for the derived keying material
 * @param label_len size of label in bytes
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_kdf(const char *kdf_algo, uint8_t out[], size_t out_len, const uint8_t secret[], size_t secret_len,
                   const uint8_t salt[], size_t salt_len, const uint8_t label[], size_t label_len);

/**
* Create a password hash using Bcrypt
* @param out buffer holding the password hash, should be of length 64 bytes
* @param out_len the desired output length in bytes
* @param password the password
* @param rng a random number generator
* @param work_factor how much work to do to slow down guessing attacks
* @param flags should be 0 in current API revision, all other uses are reserved
*       and return CRYPTO3_FFI_ERROR_BAD_FLAG
* @return 0 on success, a negative value on failure

* Output is formatted bcrypt $2a$...
*/

int nil_crypto_bcrypt_generate(uint8_t *out, size_t *out_len, const char *password, nil_crypto_rng_t rng,
                               size_t work_factor, uint32_t flags);

/*
 * Raw Block Cipher (PRP) interface
 */
typedef struct nil_crypto_block_cipher_struct *nil_crypto_block_cipher_t;

/**
 * Initialize a block cipher object
 */

int nil_crypto_block_cipher_init(nil_crypto_block_cipher_t *bc, const char *cipher_name);

/**
 * Destroy a block cipher object
 */

int nil_crypto_block_cipher_destroy(nil_crypto_block_cipher_t bc);

/**
 * Reinitializes the block cipher
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_block_cipher_clear(nil_crypto_block_cipher_t bc);

/**
 * Set the key for a block cipher instance
 */

int nil_crypto_block_cipher_set_key(nil_crypto_block_cipher_t bc, const uint8_t key[], size_t len);

/**
 * Return the positive block size of this block cipher, or negative to
 * indicate an error
 */

int nil_crypto_block_cipher_block_size(nil_crypto_block_cipher_t bc);

int nil_crypto_block_cipher_encrypt_blocks(nil_crypto_block_cipher_t bc, const uint8_t in[], uint8_t out[],
                                           size_t blocks);

int nil_crypto_block_cipher_decrypt_blocks(nil_crypto_block_cipher_t bc, const uint8_t in[], uint8_t out[],
                                           size_t blocks);

/*
 * Multiple precision integers
 */
typedef struct nil_crypto_mp_struct *nil_crypto_mp_t;

int nil_crypto_mp_init(nil_crypto_mp_t *mp);

int nil_crypto_mp_destroy(nil_crypto_mp_t mp);

// writes nil_crypto_mp_num_bytes(multiprecision)*2 + 1 bytes to out[]

int nil_crypto_mp_to_hex(const nil_crypto_mp_t mp, char *out);

int nil_crypto_mp_to_str(const nil_crypto_mp_t mp, uint8_t base, char *out, size_t *out_len);

int nil_crypto_mp_clear(nil_crypto_mp_t mp);

int nil_crypto_mp_set_from_int(nil_crypto_mp_t mp, int initial_value);

int nil_crypto_mp_set_from_mp(nil_crypto_mp_t dest, const nil_crypto_mp_t source);

int nil_crypto_mp_set_from_str(nil_crypto_mp_t dest, const char *str);

int nil_crypto_mp_set_from_radix_str(nil_crypto_mp_t dest, const char *str, size_t radix);

int nil_crypto_mp_num_bits(const nil_crypto_mp_t n, size_t *bits);

int nil_crypto_mp_num_bytes(const nil_crypto_mp_t n, size_t *bytes);

// Writes nil_crypto_mp_num_bytes(multiprecision) to vec

int nil_crypto_mp_to_bin(const nil_crypto_mp_t mp, uint8_t vec[]);

int nil_crypto_mp_from_bin(const nil_crypto_mp_t mp, const uint8_t vec[], size_t vec_len);

int nil_crypto_mp_to_uint32(const nil_crypto_mp_t mp, uint32_t *val);

/**
 * This function is not well named. Returns 1 iff mp is greater than
 * *or equal to* zero. Use nil_crypto_mp_is_negative to detect negative
 * numbers, nil_crypto_mp_is_zero to check for zero.
 */

int nil_crypto_mp_is_positive(const nil_crypto_mp_t mp);

/**
 * Return 1 iff mp is less than 0
 */

int nil_crypto_mp_is_negative(const nil_crypto_mp_t mp);

int nil_crypto_mp_flip_sign(nil_crypto_mp_t mp);
// CRYPTO3_PUBLIC_API(2,1) int nil_crypto_mp_set_negative(nil_crypto_mp_t multiprecision);

int nil_crypto_mp_is_zero(const nil_crypto_mp_t mp);

int nil_crypto_mp_is_odd(const nil_crypto_mp_t mp);

int nil_crypto_mp_is_even(const nil_crypto_mp_t mp);

int nil_crypto_mp_add(nil_crypto_mp_t result, const nil_crypto_mp_t x, const nil_crypto_mp_t y);

int nil_crypto_mp_sub(nil_crypto_mp_t result, const nil_crypto_mp_t x, const nil_crypto_mp_t y);

int nil_crypto_mp_mul(nil_crypto_mp_t result, const nil_crypto_mp_t x, const nil_crypto_mp_t y);

int nil_crypto_mp_div(nil_crypto_mp_t quotient, nil_crypto_mp_t remainder, const nil_crypto_mp_t x,
                      const nil_crypto_mp_t y);

int nil_crypto_mp_mod_mul(nil_crypto_mp_t result, const nil_crypto_mp_t x, const nil_crypto_mp_t y,
                          const nil_crypto_mp_t mod);

/*
 * Returns 0 if x != y
 * Returns 1 if x == y
 * Returns negative number on error
 */

int nil_crypto_mp_equal(const nil_crypto_mp_t x, const nil_crypto_mp_t y);

/*
 * Sets *result to comparison result:
 * -1 if x < y, 0 if x == y, 1 if x > y
 * Returns negative number on error or zero on success
 */

int nil_crypto_mp_cmp(int *result, const nil_crypto_mp_t x, const nil_crypto_mp_t y);

/*
 * Swap two nil_crypto_mp_t
 */

int nil_crypto_mp_swap(nil_crypto_mp_t x, nil_crypto_mp_t y);

// Return (core^exponent) % modulus

int nil_crypto_mp_powmod(nil_crypto_mp_t out, const nil_crypto_mp_t base, const nil_crypto_mp_t exponent,
                         const nil_crypto_mp_t modulus);

int nil_crypto_mp_lshift(nil_crypto_mp_t out, const nil_crypto_mp_t in, size_t shift);

int nil_crypto_mp_rshift(nil_crypto_mp_t out, const nil_crypto_mp_t in, size_t shift);

int nil_crypto_mp_mod_inverse(nil_crypto_mp_t out, const nil_crypto_mp_t in, const nil_crypto_mp_t modulus);

int nil_crypto_mp_rand_bits(nil_crypto_mp_t rand_out, nil_crypto_rng_t rng, size_t bits);

int nil_crypto_mp_rand_range(nil_crypto_mp_t rand_out, nil_crypto_rng_t rng, const nil_crypto_mp_t lower_bound,
                             const nil_crypto_mp_t upper_bound);

int nil_crypto_mp_gcd(nil_crypto_mp_t out, const nil_crypto_mp_t x, const nil_crypto_mp_t y);

/**
 * Returns 0 if n is not prime
 * Returns 1 if n is prime
 * Returns negative number on error
 */

int nil_crypto_mp_is_prime(const nil_crypto_mp_t n, nil_crypto_rng_t rng, size_t test_prob);

/**
 * Returns 0 if specified bit of n is not set
 * Returns 1 if specified bit of n is set
 * Returns negative number on error
 */

int nil_crypto_mp_get_bit(const nil_crypto_mp_t n, size_t bit);

/**
 * Set the specified bit
 */

int nil_crypto_mp_set_bit(nil_crypto_mp_t n, size_t bit);

/**
 * Clear the specified bit
 */

int nil_crypto_mp_clear_bit(nil_crypto_mp_t n, size_t bit);

/* Bcrypt password hashing */

/**
 * Check a previously created password hash
 * @param pass the password to check against
 * @param hash the stored hash to check against
 * @return 0 if if this password/hash combination is valid,
 *       1 if the combination is not valid (but otherwise well formed),
 *       negative on error
 */

int nil_crypto_bcrypt_is_valid(const char *pass, const char *hash);

/*
 * Public/private key creation, import, ...
 */
typedef struct nil_crypto_privkey_struct *nil_crypto_privkey_t;

int nil_crypto_privkey_create(nil_crypto_privkey_t *key, const char *algo_name, const char *algo_params,
                              nil_crypto_rng_t rng);

#define CRYPTO3_CHECK_KEY_EXPENSIVE_TESTS 1

int nil_crypto_privkey_check_key(nil_crypto_privkey_t key, nil_crypto_rng_t rng, uint32_t flags);

int nil_crypto_privkey_create_rsa(nil_crypto_privkey_t *key, nil_crypto_rng_t rng, size_t n_bits);

int nil_crypto_privkey_create_ecdsa(nil_crypto_privkey_t *key, nil_crypto_rng_t rng, const char *params);

int nil_crypto_privkey_create_ecdh(nil_crypto_privkey_t *key, nil_crypto_rng_t rng, const char *params);

int nil_crypto_privkey_create_mceliece(nil_crypto_privkey_t *key, nil_crypto_rng_t rng, size_t n, size_t t);

int nil_crypto_privkey_create_dh(nil_crypto_privkey_t *key, nil_crypto_rng_t rng, const char *param);

/*
 * Generates DSA key pair. Gives to a caller control over key length
 * and order of a subgroup 'q'.
 *
 * @param   key   handler to the resulting key
 * @param   random   initialized PRNG
 * @param   pbits length of the key in bits. Must be between in range (1024, 3072)
 *          and multiple of 64. Bit size of the prime 'p'
 * @param   qbits order of the subgroup. Must be in range (160, 256) and multiple
 *          of 8
 *
 * @returns CRYPTO3_FFI_SUCCESS Success, `key' initialized with DSA key
 * @returns CRYPTO3_FFI_ERROR_NULL_POINTER  either `key' or `random' is NULL
 * @returns CRYPTO3_FFI_ERROR_BAD_PARAMETER unexpected value for either `pbits' or
 *          `qbits'
 * @returns CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED functionality not implemented
 *
-------------------------------------------------------------------------------- */

int nil_crypto_privkey_create_dsa(nil_crypto_privkey_t *key, nil_crypto_rng_t rng, size_t pbits, size_t qbits);

/*
 * Generates ElGamal key pair. Caller has a control over key length
 * and order of a subgroup 'q'. Function is able to use two types of
 * primes:
 *    * if pbits-1 == qbits then safe primes are used for key generation
 *    * otherwise generation uses group of prime order
 *
 * @param   key   handler to the resulting key
 * @param   random   initialized PRNG
 * @param   pbits length of the key in bits. Must be at least 1024
 * @param   qbits order of the subgroup. Must be at least 160
 *
 * @returns CRYPTO3_FFI_SUCCESS Success, `key' initialized with DSA key
 * @returns CRYPTO3_FFI_ERROR_NULL_POINTER  either `key' or `random' is NULL
 * @returns CRYPTO3_FFI_ERROR_BAD_PARAMETER unexpected value for either `pbits' or
 *          `qbits'
 * @returns CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED functionality not implemented
 *
-------------------------------------------------------------------------------- */

int nil_crypto_privkey_create_elgamal(nil_crypto_privkey_t *key, nil_crypto_rng_t rng, size_t pbits, size_t qbits);

/*
 * Input currently assumed to be PKCS #8 structure;
 * Set password to NULL to indicate no encryption expected
 */

int nil_crypto_privkey_load(nil_crypto_privkey_t *key, nil_crypto_rng_t rng, const uint8_t bits[], size_t len,
                            const char *password);

int nil_crypto_privkey_destroy(nil_crypto_privkey_t key);

#define CRYPTO3_PRIVKEY_EXPORT_FLAG_DER 0
#define CRYPTO3_PRIVKEY_EXPORT_FLAG_PEM 1

/*
 * On input *out_len is number of bytes in out[]
 * On output *out_len is number of bytes written (or required)
 * If out is not big enough no output is written, *out_len is set and 1 is returned
 * Returns 0 on success and sets
 * If some other error occurs a negative integer is returned.
 */

int nil_crypto_privkey_export(nil_crypto_privkey_t key, uint8_t out[], size_t *out_len, uint32_t flags);

/*
 * Export a private key, running PBKDF for specified amount of time
 * @param key the private key to export
 */

int nil_crypto_privkey_export_encrypted_pbkdf_msec(nil_crypto_privkey_t key, uint8_t out[], size_t *out_len,
                                                   nil_crypto_rng_t rng, const char *passphrase,
                                                   uint32_t pbkdf_msec_runtime, size_t *pbkdf_iterations_out,
                                                   const char *cipher_algo, const char *pbkdf_algo, uint32_t flags);

/*
 * Export a private key using the specified number of iterations.
 */

int nil_crypto_privkey_export_encrypted_pbkdf_iter(nil_crypto_privkey_t key, uint8_t out[], size_t *out_len,
                                                   nil_crypto_rng_t rng, const char *passphrase,
                                                   size_t pbkdf_iterations, const char *cipher_algo,
                                                   const char *pbkdf_algo, uint32_t flags);

typedef struct nil_crypto_pubkey_struct *nil_crypto_pubkey_t;

int nil_crypto_pubkey_load(nil_crypto_pubkey_t *key, const uint8_t bits[], size_t len);

int nil_crypto_privkey_export_pubkey(nil_crypto_pubkey_t *out, nil_crypto_privkey_t in);

int nil_crypto_pubkey_export(nil_crypto_pubkey_t key, uint8_t out[], size_t *out_len, uint32_t flags);

int nil_crypto_pubkey_algo_name(nil_crypto_pubkey_t key, char out[], size_t *out_len);

/**
 * Returns 0 if key is valid, negative if invalid key or some other error
 */

int nil_crypto_pubkey_check_key(nil_crypto_pubkey_t key, nil_crypto_rng_t rng, uint32_t flags);

int nil_crypto_pubkey_estimated_strength(nil_crypto_pubkey_t key, size_t *estimate);

int nil_crypto_pubkey_fingerprint(nil_crypto_pubkey_t key, const char *hash, uint8_t out[], size_t *out_len);

int nil_crypto_pubkey_destroy(nil_crypto_pubkey_t key);

/*
 * Get arbitrary named fields from public or privat keys
 */

int nil_crypto_pubkey_get_field(nil_crypto_mp_t output, nil_crypto_pubkey_t key, const char *field_name);

int nil_crypto_privkey_get_field(nil_crypto_mp_t output, nil_crypto_privkey_t key, const char *field_name);

/*
 * Algorithm specific key operations: RSA
 */

int nil_crypto_privkey_load_rsa(nil_crypto_privkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t q, nil_crypto_mp_t e);

int nil_crypto_privkey_rsa_get_p(nil_crypto_mp_t p, nil_crypto_privkey_t rsa_key);

int nil_crypto_privkey_rsa_get_q(nil_crypto_mp_t q, nil_crypto_privkey_t rsa_key);

int nil_crypto_privkey_rsa_get_d(nil_crypto_mp_t d, nil_crypto_privkey_t rsa_key);

int nil_crypto_privkey_rsa_get_n(nil_crypto_mp_t n, nil_crypto_privkey_t rsa_key);

int nil_crypto_privkey_rsa_get_e(nil_crypto_mp_t e, nil_crypto_privkey_t rsa_key);

int nil_crypto_pubkey_load_rsa(nil_crypto_pubkey_t *key, nil_crypto_mp_t n, nil_crypto_mp_t e);

int nil_crypto_pubkey_rsa_get_e(nil_crypto_mp_t e, nil_crypto_pubkey_t rsa_key);

int nil_crypto_pubkey_rsa_get_n(nil_crypto_mp_t n, nil_crypto_pubkey_t rsa_key);

/*
 * Algorithm specific key operations: DSA
 */

int nil_crypto_privkey_load_dsa(nil_crypto_privkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t q, nil_crypto_mp_t g,
                                nil_crypto_mp_t x);

int nil_crypto_pubkey_load_dsa(nil_crypto_pubkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t q, nil_crypto_mp_t g,
                               nil_crypto_mp_t y);

int nil_crypto_privkey_dsa_get_x(nil_crypto_mp_t n, nil_crypto_privkey_t key);

int nil_crypto_pubkey_dsa_get_p(nil_crypto_mp_t p, nil_crypto_pubkey_t key);

int nil_crypto_pubkey_dsa_get_q(nil_crypto_mp_t q, nil_crypto_pubkey_t key);

int nil_crypto_pubkey_dsa_get_g(nil_crypto_mp_t d, nil_crypto_pubkey_t key);

int nil_crypto_pubkey_dsa_get_y(nil_crypto_mp_t y, nil_crypto_pubkey_t key);

/*
 * Loads Diffie Hellman private key
 *
 * @param key variable populated with key material
 * @param p prime order of a Z_p group
 * @param g group generator
 * @param x private key
 *
 * @pre key is NULL on input
 * @post function allocates memory and assigns to `key'
 *
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_privkey_load_dh(nil_crypto_privkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t g, nil_crypto_mp_t x);

/*
 * Loads Diffie Hellman public key
 *
 * @param key variable populated with key material
 * @param p prime order of a Z_p group
 * @param g group generator
 * @param y public key
 *
 * @pre key is NULL on input
 * @post function allocates memory and assigns to `key'
 *
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_pubkey_load_dh(nil_crypto_pubkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t g, nil_crypto_mp_t y);

/*
 * Algorithm specific key operations: ElGamal
 */

/*
 * Loads ElGamal public key
 * @param key variable populated with key material
 * @param p prime order of a Z_p group
 * @param g group generator
 * @param y public key
 *
 * @pre key is NULL on input
 * @post function allocates memory and assigns to `key'
 *
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_pubkey_load_elgamal(nil_crypto_pubkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t g, nil_crypto_mp_t y);

/*
 * Loads ElGamal private key
 *
 * @param key variable populated with key material
 * @param p prime order of a Z_p group
 * @param g group generator
 * @param x private key
 *
 * @pre key is NULL on input
 * @post function allocates memory and assigns to `key'
 *
 * @return 0 on success, a negative value on failure
 */

int nil_crypto_privkey_load_elgamal(nil_crypto_privkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t g, nil_crypto_mp_t x);

/*
 * Algorithm specific key operations: Ed25519
 */

int nil_crypto_privkey_load_ed25519(nil_crypto_privkey_t *key, const uint8_t privkey[32]);

int nil_crypto_pubkey_load_ed25519(nil_crypto_pubkey_t *key, const uint8_t pubkey[32]);

int nil_crypto_privkey_ed25519_get_privkey(nil_crypto_privkey_t key, uint8_t output[64]);

int nil_crypto_pubkey_ed25519_get_pubkey(nil_crypto_pubkey_t key, uint8_t pubkey[32]);

/*
 * Algorithm specific key operations: ECDSA and ECDH
 */

int nil_crypto_privkey_load_ecdsa(nil_crypto_privkey_t *key, const nil_crypto_mp_t scalar, const char *curve_name);

int nil_crypto_pubkey_load_ecdsa(nil_crypto_pubkey_t *key, const nil_crypto_mp_t public_x,
                                 const nil_crypto_mp_t public_y, const char *curve_name);

int nil_crypto_pubkey_load_ecdh(nil_crypto_pubkey_t *key, const nil_crypto_mp_t public_x,
                                const nil_crypto_mp_t public_y, const char *curve_name);

int nil_crypto_privkey_load_ecdh(nil_crypto_privkey_t *key, const nil_crypto_mp_t scalar, const char *curve_name);

int nil_crypto_pubkey_load_sm2(nil_crypto_pubkey_t *key, const nil_crypto_mp_t public_x, const nil_crypto_mp_t public_y,
                               const char *curve_name);

int nil_crypto_privkey_load_sm2(nil_crypto_privkey_t *key, const nil_crypto_mp_t scalar, const char *curve_name);

int nil_crypto_pubkey_load_sm2_enc(nil_crypto_pubkey_t *key, const nil_crypto_mp_t public_x,
                                   const nil_crypto_mp_t public_y, const char *curve_name);

int nil_crypto_privkey_load_sm2_enc(nil_crypto_privkey_t *key, const nil_crypto_mp_t scalar, const char *curve_name);

int nil_crypto_pubkey_sm2_compute_za(uint8_t out[], size_t *out_len, const char *ident, const char *hash_algo,
                                     const nil_crypto_pubkey_t key);

/*
 * Public Key encryption
 */
typedef struct nil_crypto_pk_op_encrypt_struct *nil_crypto_pk_op_encrypt_t;

int nil_crypto_pk_op_encrypt_create(nil_crypto_pk_op_encrypt_t *op, nil_crypto_pubkey_t key, const char *padding,
                                    uint32_t flags);

int nil_crypto_pk_op_encrypt_destroy(nil_crypto_pk_op_encrypt_t op);

int nil_crypto_pk_op_encrypt(nil_crypto_pk_op_encrypt_t op, nil_crypto_rng_t rng, uint8_t out[], size_t *out_len,
                             const uint8_t plaintext[], size_t plaintext_len);

/*
 * Public Key decryption
 */
typedef struct nil_crypto_pk_op_decrypt_struct *nil_crypto_pk_op_decrypt_t;

int nil_crypto_pk_op_decrypt_create(nil_crypto_pk_op_decrypt_t *op, nil_crypto_privkey_t key, const char *padding,
                                    uint32_t flags);

int nil_crypto_pk_op_decrypt_destroy(nil_crypto_pk_op_decrypt_t op);

int nil_crypto_pk_op_decrypt(nil_crypto_pk_op_decrypt_t op, uint8_t out[], size_t *out_len, const uint8_t ciphertext[],
                             size_t ciphertext_len);

/*
 * signature Generation
 */
typedef struct nil_crypto_pk_op_sign_struct *nil_crypto_pk_op_sign_t;

int nil_crypto_pk_op_sign_create(nil_crypto_pk_op_sign_t *op, nil_crypto_privkey_t key, const char *hash_and_padding,
                                 uint32_t flags);

int nil_crypto_pk_op_sign_destroy(nil_crypto_pk_op_sign_t op);

int nil_crypto_pk_op_sign_update(nil_crypto_pk_op_sign_t op, const uint8_t in[], size_t in_len);

int nil_crypto_pk_op_sign_finish(nil_crypto_pk_op_sign_t op, nil_crypto_rng_t rng, uint8_t sig[], size_t *sig_len);

/*
 * signature verification
 */
typedef struct nil_crypto_pk_op_verify_struct *nil_crypto_pk_op_verify_t;

int nil_crypto_pk_op_verify_create(nil_crypto_pk_op_verify_t *op, nil_crypto_pubkey_t key, const char *hash_and_padding,
                                   uint32_t flags);

int nil_crypto_pk_op_verify_destroy(nil_crypto_pk_op_verify_t op);

int nil_crypto_pk_op_verify_update(nil_crypto_pk_op_verify_t op, const uint8_t in[], size_t in_len);

int nil_crypto_pk_op_verify_finish(nil_crypto_pk_op_verify_t op, const uint8_t sig[], size_t sig_len);

/*
 * Key Agreement
 */
typedef struct nil_crypto_pk_op_ka_struct *nil_crypto_pk_op_ka_t;

int nil_crypto_pk_op_key_agreement_create(nil_crypto_pk_op_ka_t *op, nil_crypto_privkey_t key, const char *kdf,
                                          uint32_t flags);

int nil_crypto_pk_op_key_agreement_destroy(nil_crypto_pk_op_ka_t op);

int nil_crypto_pk_op_key_agreement_export_public(nil_crypto_privkey_t key, uint8_t out[], size_t *out_len);

int nil_crypto_pk_op_key_agreement(nil_crypto_pk_op_ka_t op, uint8_t out[], size_t *out_len, const uint8_t other_key[],
                                   size_t other_key_len, const uint8_t salt[], size_t salt_len);

int nil_crypto_pkcs_hash_id(const char *hash_name, uint8_t pkcs_id[], size_t *pkcs_id_len);

/*
 *
 * @param mce_key must be a McEliece key
 * ct_len should be pt_len + n/8 + a few?
 */

int nil_crypto_mceies_encrypt(nil_crypto_pubkey_t mce_key, nil_crypto_rng_t rng, const char *aead, const uint8_t pt[],
                              size_t pt_len, const uint8_t ad[], size_t ad_len, uint8_t ct[], size_t *ct_len);

int nil_crypto_mceies_decrypt(nil_crypto_privkey_t mce_key, const char *aead, const uint8_t ct[], size_t ct_len,
                              const uint8_t ad[], size_t ad_len, uint8_t pt[], size_t *pt_len);


#ifdef __cplusplus
}
#endif

#endif
