add_custom_target(compile_rust_examples)
add_custom_target(rust_examples_generate_crct)
add_custom_target(rust_examples_generate_tbl)
add_custom_target(rust_examples_generate_both)
add_custom_target(prove_rust_examples)

set(RUSTC "${CMAKE_BINARY_DIR}/libs/rslang/build/host/stage${RSLANG_BUILD_STAGE}/bin/rustc")
if(${RSLANG_BUILD_EXTENDED})
    set(CARGO "${CMAKE_BINARY_DIR}/libs/rslang/build/host/stage${RSLANG_BUILD_STAGE}-tools-bin/cargo")
else()
    set(CARGO "${CMAKE_BINARY_DIR}/libs/rslang/target/release/cargo")
endif()
set(TARGET "assigner-unknown-unknown")
set(TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/target")
set(EXAMPLES_BINARY_DIR "${TARGET_DIR}/${TARGET}/release/examples")

function(add_rust_example_without_proving example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues EXAMPLE_NAME INPUT PRIVATE_INPUT CURVE_TYPE MAX_NUM_PROVERS)
    set(multiValues)
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

    add_dependencies(compile_rust_examples "${example_target}")

    set(binary_name ${EXAMPLES_BINARY_DIR}/${ARG_EXAMPLE_NAME}.ll)

    if(NOT DEFINED ARG_PRIVATE_INPUT)
        set(ARG_PRIVATE_INPUT "none")
    endif()

    if(NOT DEFINED ARG_MAX_NUM_PROVERS)
        set(ARG_MAX_NUM_PROVERS 0)
    endif()

    if(NOT DEFINED ARG_ARITHMETIZARION)
        set(ARG_ARITHMETIZARION "none")
    endif()

    assign_ir(${example_target} ${binary_name} ${ARG_INPUT} ${ARG_PRIVATE_INPUT} ${ARG_CURVE_TYPE} ${ARG_ARITHMETIZARION} ${ARG_MAX_NUM_PROVERS})

    add_dependencies(rust_examples_generate_crct ${example_target}_generate_crct)
    add_dependencies(rust_examples_generate_tbl ${example_target}_generate_tbl)
    add_dependencies(rust_examples_generate_both ${example_target}_generate_both)
endfunction()

function(add_rust_example_with_proving example_target)
    set(prefix ARG)
    set(noValues "")
    set(singleValues EXAMPLE_NAME INPUT CURVE_TYPE TBL_GEN_SPEED)
    set(multiValues)
    cmake_parse_arguments(${prefix}
                        "${noValues}"
                        "${singleValues}"
                        "${multiValues}"
                        ${ARGN})

    add_rust_example_without_proving(${example_target} ${ARGN})

    if(NOT DEFINED ARG_MAX_NUM_PROVERS)
        set(ARG_MAX_NUM_PROVERS 0)
    endif()

    if(NOT DEFINED ARG_TBL_GEN_SPEED)
        set(ARG_TBL_GEN_SPEED "slow")
    endif()

    gen_proof(${example_target} ${ARG_CURVE_TYPE} ${ARG_MAX_NUM_PROVERS} ${ARG_TBL_GEN_SPEED})
    add_dependencies(prove_rust_examples ${example_target}_prove)

    if(GENERATE_EVM_VERIFIER)
        gen_evm_verifier(${example_target} ${ARG_CURVE_TYPE} ${ARG_MAX_NUM_PROVERS})
        add_dependencies(prove_rust_examples ${example_target}_evm_verifier)
    endif()


    endfunction()

add_rust_example_with_proving(fibonacci_rust_example EXAMPLE_NAME fibonacci INPUT fibonacci.inp CURVE_TYPE pallas)
add_rust_example_with_proving(arithmetics_builtins_rust_example EXAMPLE_NAME arithmetics_builtins INPUT arithmetics.inp CURVE_TYPE pallas)
add_rust_example_with_proving(arithmetics_arkworks_rust_example EXAMPLE_NAME arithmetics_arkworks INPUT arithmetics.inp CURVE_TYPE pallas)
add_rust_example_with_proving(ed25519_curve_add_rust_example EXAMPLE_NAME ed25519_curve_add INPUT ed25519_curve_add.inp CURVE_TYPE pallas)

add_rust_example_without_proving(eddsa_signature_verification_rust_example EXAMPLE_NAME eddsa_signature_verification INPUT eddsa_signature_verification_rust.inp CURVE_TYPE pallas)
add_rust_example_without_proving(eddsa_signature_verification_builtins_rust_example EXAMPLE_NAME eddsa_signature_verification_builtins INPUT eddsa_signature_verification_rust.inp CURVE_TYPE pallas)
add_rust_example_without_proving(merkle_tree_sha2_256_rust_example EXAMPLE_NAME merkle_tree_sha2_256 INPUT merkle_tree_sha2_256_rust.inp CURVE_TYPE pallas)
add_rust_example_without_proving(sha2_256_rust_example EXAMPLE_NAME sha2_256 INPUT sha2_256_rust.inp CURVE_TYPE pallas)
add_rust_example_without_proving(sha2_512_rust_example EXAMPLE_NAME sha2_512 INPUT sha2_512_rust.inp CURVE_TYPE pallas)
add_rust_example_without_proving(validate_merkle_path_sha2_256_rust_example EXAMPLE_NAME validate_merkle_path_sha2_256 INPUT validate_merkle_path_sha2_256_rust.inp CURVE_TYPE pallas)
