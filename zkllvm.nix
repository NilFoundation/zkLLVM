{ lib,
  stdenv,
  src_repo,
  pkgs,
  ninja,
  pkg-config,
  cmake,
  # We'll use boost183 by default, but you can override it
  boost_lib ? pkgs.boost183,
  gdb,
  cmake_modules,
  crypto3,
  blueprint,
  python3,
  git,
  cargo,
  openssl,
  enableDebugging,
  enableDebug ? false,
  enableTesting ? false
  }:
let
  inherit (lib) optional;
in stdenv.mkDerivation {
  name = "Zkllvm";
  src = src_repo;

  nativeBuildInputs = [ cmake ninja pkg-config ] ++ (lib.optional (!stdenv.isDarwin) gdb);

  # enableDebugging will keep debug symbols in boost
  propagatedBuildInputs = [ (if enableDebug then (enableDebugging boost_lib) else boost_lib) ];

  buildInputs = [crypto3 blueprint cmake_modules python3 git cargo openssl];

  cmakeFlags =
    [
      (if enableTesting then "-DENABLE_TESTS=TRUE" else "")
      (if enableTesting then "-DBUILD_TESTS=TRUE" else "")
      (if enableTesting then "-DCMAKE_ENABLE_TESTS=TRUE" else "")

      (if enableDebug then "-DCMAKE_BUILD_TYPE=Debug" else "-DCMAKE_BUILD_TYPE=Release")
      (if enableDebug then "-DCMAKE_CXX_FLAGS=-ggdb" else "")
      "-DZKLLVM_VERSION=0.1.18"
      "-DGENERATE_EVM_VERIFIER=TRUE"
    ];

  ninjaFlags = "assigner clang transpiler";
  dontFixCmake = true;

  doCheck = enableTesting;

  checkPhase = ''
    echo "pwd:"
    echo `pwd`
    ls ../
    bash ../run_tests.sh
  '';

  shellHook = ''
    echo "Welcome to zkllvm development environment!"
  '';
}
