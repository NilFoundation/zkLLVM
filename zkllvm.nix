{ lib,
  stdenv,
  src_repo,
  pkgs,
  ninja,
  pkg-config,
  cmake,
  boost183,
  # We'll use boost183 by default, but you can override it
  boost_lib ? boost183,
  gdb,
  cmake_modules,
  crypto3,
  blueprint,
  enableDebugging,
  enableDebug ? false,
  runTests ? false,
  }:
let
  inherit (lib) optional;
in stdenv.mkDerivation {
  name = "Zkllvm";
  src = src_repo;

  nativeBuildInputs = [ cmake ninja pkg-config ] ++ (lib.optional (!stdenv.isDarwin) gdb);

  # enableDebugging will keep debug symbols in boost
  propagatedBuildInputs = [ (if enableDebug then (enableDebugging boost_lib) else boost_lib) ];

  buildInputs = [crypto3 blueprint cmake_modules pkgs.python3 pkgs.git pkgs.cargo pkgs.openssl];

  cmakeFlags =
    [
      (if runTests then "-DENABLE_TESTS=TRUE" else "")
      (if runTests then "-DBUILD_TESTS=TRUE" else "")
      (if runTests then "-DCMAKE_ENABLE_TESTS=TRUE" else "")
      (if enableDebug then "-DCMAKE_BUILD_TYPE=Debug" else "-DCMAKE_BUILD_TYPE=Release")
      (if enableDebug then "-DCMAKE_CXX_FLAGS=-ggdb" else "")
      "-DZKLLVM_VERSION=0.1.18"
      "-DGENERATE_EVM_VERIFIER=TRUE"
    ];

  ninjaFlags = "assigner clang transpiler";

        integrationTestingTargets = [
            "arithmetics_cpp_example"
            "polynomial_cpp_example"
            "poseidon_cpp_example"
            "merkle_tree_poseidon_cpp_example"
            "uint_remainder_cpp"
            "uint_shift_left"
            "uint_bit_decomposition"
            "uint_bit_composition"
            "compare_eq_cpp"
            "private_input_cpp"
        ];

        testList = [
            "compile_cpp_examples"
            "cpp_examples_generate_crct"
            "cpp_examples_generate_tbl_no_check"
            "cpp_examples_generate_both"
            "cpp_examples_estimate_size"
            "all_tests_compile_as_cpp_code"
            "all_tests_compile_as_circuits"
            "all_tests_run_expected_res_calculation"
            "all_tests_assign_circuits"
            "check-crypto3-assigner"
            "prove_cpp_examples"
            "recursive_gen"
            "compile_and_run_transpiler_tests"
            "recursion"
        ];

  doCheck = runTests;
  doInstall = false;

        checkPhase = ''
          ls -l -a
          cp * ${placeholder "out"}/build-result;
        '';

  shellHook = ''
    echo "Welcome to zkllvm development environment!"
  '';
}
