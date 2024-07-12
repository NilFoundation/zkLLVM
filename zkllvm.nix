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
      (if runTests then "-DBUILD_TESTS=TRUE" else "")
      (if runTests then "-DCMAKE_ENABLE_TESTS=TRUE" else "")
      (if enableDebug then "-DCMAKE_BUILD_TYPE=Debug" else "-DCMAKE_BUILD_TYPE=Release")
      (if enableDebug then "-DCMAKE_CXX_FLAGS=-ggdb" else "")
      "-DZKLLVM_VERSION=0.1.18"
      "-DGENERATE_EVM_VERIFIER=TRUE"
    ];

  ninjaFlags = "assigner clang transpiler";

  doCheck = false;

  shellHook = ''
    echo "Welcome to zkllvm development environment!"
  '';
}
