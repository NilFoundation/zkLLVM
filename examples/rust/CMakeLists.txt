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
                      RUSTC=${RUSTC} ${CARGO} build --target-dir ${TARGET_DIR} --target ${TARGET} --example ${ARG_EXAMPLE_NAME} --release
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

add_rust_example(arithmetics_raw_rust_example EXAMPLE_NAME arithmetics_raw INPUT arithmetics.inp)
