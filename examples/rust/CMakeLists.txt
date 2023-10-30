add_custom_target(circuit_rust_examples)
add_custom_target(assign_rust_examples)
add_custom_target(prove_rust_examples)

set(RUSTC "${CMAKE_BINARY_DIR}/libs/rslang/build/host/stage1/bin/rustc")
set(CARGO "${CMAKE_BINARY_DIR}/libs/rslang/target/release/cargo")
set(TARGET "assigner-unknown-unknown")
set(TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/target")

function(add_rust_example example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues INPUT)
    set(multiValues EXAMPLE_NAME)
    cmake_parse_arguments(${prefix}
                        "${noValues}"
                        "${singleValues}"
                        "${multiValues}"
                        ${ARGN})

    add_custom_target(${example_target}
                      RUSTC=${RUSTC} ${CARGO} build --target-dir ${TARGET_DIR} --target ${TARGET} --example ${ARG_EXAMPLE_NAME} --release --ignore-rust-version
                      DEPENDS rslang
                      USES_TERMINAL
                      VERBATIM
                      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_dependencies(circuit_rust_examples "${example_target}")

    set(binary_name ${TARGET_DIR}/${TARGET}/release/examples/${ARG_EXAMPLE_NAME}.ll)

    assign_ir(${example_target} ${binary_name} ${ARG_INPUT})
    add_dependencies(assign_rust_examples ${example_target}_assign)

    gen_proof(${example_target} ${ARG_INPUT})
    add_dependencies(prove_rust_examples ${example_target}_prove)
endfunction()

add_rust_example(arithmetics_builtins_rust_example EXAMPLE_NAME arithmetics_builtins INPUT arithmetics.inp)
add_rust_example(arithmetics_arkworks_rust_example EXAMPLE_NAME arithmetics_arkworks INPUT arithmetics_arkworks.inp)
add_rust_example(eddsa_signature_verification_rust_example EXAMPLE_NAME eddsa_signature_verification INPUT eddsa_signature_verification_rust.inp)
add_rust_example(merkle_tree_sha2_256_rust_example EXAMPLE_NAME merkle_tree_sha2_256 INPUT merkle_tree_sha2_256_rust.inp)
add_rust_example(sha2_256_rust_example EXAMPLE_NAME sha2_256 INPUT sha2_256_rust.inp)
add_rust_example(sha2_512_rust_example EXAMPLE_NAME sha2_512 INPUT sha2_512_rust.inp)
