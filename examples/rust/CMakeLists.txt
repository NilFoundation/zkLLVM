add_custom_target(circuit_rust_examples)
add_custom_target(assign_rust_examples)
add_custom_target(prove_rust_examples)

set(RUSTC "${CMAKE_BINARY_DIR}/libs/rslang/build/host/stage${RSLANG_BUILD_STAGE}/bin/rustc")
if(${RSLANG_BUILD_EXTENDED})
    set(CARGO "${CMAKE_BINARY_DIR}/libs/rslang/build/host/stage${RSLANG_BUILD_STAGE}-tools-bin/cargo")
else()
    set(CARGO "${CMAKE_BINARY_DIR}/libs/rslang/target/release/cargo")
endif()
set(TARGET "assigner-unknown-unknown")
set(TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/target")

function(add_rust_example_without_proving example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues INPUT CURVE_TYPE)
    set(multiValues EXAMPLE_NAME)
    cmake_parse_arguments(${prefix}
                        "${noValues}"
                        "${singleValues}"
                        "${multiValues}"
                        ${ARGN})

    add_custom_target(${example_target}
                      RUSTC=${RUSTC} ${CARGO} build --target-dir ${TARGET_DIR} --target ${TARGET} --example ${ARG_EXAMPLE_NAME} --release
                      DEPENDS rslang
                      USES_TERMINAL
                      VERBATIM
                      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_dependencies(circuit_rust_examples "${example_target}")

    set(binary_name ${TARGET_DIR}/${TARGET}/release/examples/${ARG_EXAMPLE_NAME}.ll)

    assign_ir(${example_target} ${binary_name} ${ARG_INPUT} ${ARG_CURVE_TYPE})
    add_dependencies(assign_rust_examples ${example_target}_assign)
endfunction()

function(add_rust_example_with_proving example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues INPUT CURVE_TYPE)
    set(multiValues EXAMPLE_NAME)
    cmake_parse_arguments(${prefix}
                        "${noValues}"
                        "${singleValues}"
                        "${multiValues}"
                        ${ARGN})

    add_rust_example_without_proving(${example_target} ${ARGN})

    gen_proof(${example_target} ${ARG_INPUT} ${ARG_CURVE_TYPE})
    add_dependencies(prove_rust_examples ${example_target}_prove)
endfunction()

add_rust_example_with_proving(arithmetics_builtins_rust_example EXAMPLE_NAME arithmetics_builtins INPUT arithmetics.inp CURVE_TYPE pallas)
add_rust_example_with_proving(arithmetics_arkworks_rust_example EXAMPLE_NAME arithmetics_arkworks INPUT arithmetics.inp CURVE_TYPE pallas)
add_rust_example_with_proving(ed25519_curve_add_rust_example EXAMPLE_NAME ed25519_curve_add INPUT ed25519_curve_add.inp CURVE_TYPE pallas)

add_rust_example_without_proving(eddsa_signature_verification_rust_example EXAMPLE_NAME eddsa_signature_verification INPUT eddsa_signature_verification_rust.inp CURVE_TYPE pallas)
add_rust_example_without_proving(eddsa_signature_verification_builtins_rust_example EXAMPLE_NAME eddsa_signature_verification_builtins INPUT eddsa_signature_verification_rust.inp CURVE_TYPE pallas)
add_rust_example_without_proving(merkle_tree_sha2_256_rust_example EXAMPLE_NAME merkle_tree_sha2_256 INPUT merkle_tree_sha2_256_rust.inp CURVE_TYPE pallas)
add_rust_example_without_proving(sha2_256_rust_example EXAMPLE_NAME sha2_256 INPUT sha2_256_rust.inp CURVE_TYPE pallas)
add_rust_example_without_proving(sha2_512_rust_example EXAMPLE_NAME sha2_512 INPUT sha2_512_rust.inp CURVE_TYPE pallas)
add_rust_example_without_proving(validate_merkle_path_sha2_256_rust_example EXAMPLE_NAME validate_merkle_path_sha2_256 INPUT validate_merkle_path_sha2_256_rust.inp CURVE_TYPE pallas)
