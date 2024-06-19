{
  description = "Nix flake for zkEVM";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
    nix-3rdparty = {
      url = "github:NilFoundation/nix-3rdparty";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-utils.follows = "flake-utils";
      };
    };
    nil-crypto3 = {
      url = "https://github.com/NilFoundation/crypto3";
      type = "git";
      submodules = true;
      inputs = {
        nixpkgs.follows = "nixpkgs";
      };
    };
    nil-zkllvm-blueprint = {
      url = "https://github.com/NilFoundation/zkllvm-blueprint";
      type = "git";
      submodules = true;
      inputs = {
        nixpkgs.follows = "nixpkgs";
        nil_crypto3.follows = "nil-crypto3";
      };
    };
  };

  outputs =
    { self
    , nixpkgs
    , flake-utils
    , nix-3rdparty
    , nil-crypto3
    , nil-zkllvm-blueprint
    }:
    flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs {
        overlays = [ nix-3rdparty.overlays.${system}.default ];
        inherit system;
      };
      crypto3 = nil-crypto3.packages.${system}.default;
      blueprint = nil-zkllvm-blueprint.packages.${system}.default;

      # Default env will bring us GCC 13 as default compiler
      stdenv = pkgs.stdenv;

      defaultNativeBuildInputs = [
        pkgs.cmake
        pkgs.ninja
        pkgs.python3
        pkgs.git
      ];

      defaultBuildInputs = [
        # Default nixpkgs packages
        pkgs.boost
        pkgs.valijson
        # Packages from nix-3rdparty
        #pkgs.sszpp
        #pkgs.evmc
        # Repo dependencies
        #evm_assigner
        crypto3
        blueprint
      ];

      defaultCheckInputs = [
        pkgs.gtest
      ];

      defaultDevTools = [
        pkgs.doxygen
        pkgs.clang_17 # clang-format and clang-tidy
        pkgs.go_1_22
        pkgs.gotools
        pkgs.go-tools
        pkgs.gopls
        pkgs.golangci-lint
        pkgs.gofumpt
        pkgs.gci
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

      defaultCmakeFlags = [
        "-DCMAKE_CXX_STANDARD=17"
        "-DBUILD_SHARED_LIBS=TRUE"
        "-DZKLLVM_VERSION=1.2.3" # TODO change this
      ];

      releaseBuild = stdenv.mkDerivation {
        name = "zkLLVM";
        cmakeBuildType = "Release";
        buildInputs = defaultBuildInputs ++ defaultNativeBuildInputs;

        buildPhase = ''
          cmake --build . -t assigner clang transpiler
        '';

        src = self;

        doCheck = false;
      };

      # TODO: we need to propagate debug mode to dependencies here:
      debugBuild = releaseBuild.overrideAttrs (finalAttrs: previousAttrs: {
        name = previousAttrs.name + "-debug";
        cmakeBuildType = "Debug";
        buildInputs = defaultBuildInputs ++ defaultNativeBuildInputs;
      });

      testBuild = buildType: buildType.overrideAttrs (finalAttrs: previousAttrs: {
        name = previousAttrs.name + "-tests";
        buildInputs = defaultBuildInputs ++ defaultNativeBuildInputs;

        cmakeFlags = defaultCmakeFlags ++ [
          "-DENABLE_TESTS=TRUE"
          "-DBUILD_TEST=TRUE"
          "-DCMAKE_ENABLE_TESTS=TRUE"
        ];

        ninjaFlags = pkgs.lib.strings.concatStringsSep " " (["-k 0"] ++ testList);

        doCheck = true;

        checkPhase = ''
          # JUNIT file without explicit file name is generated after the name of the master test suite inside `CMAKE_CURRENT_SOURCE_DIR` (/build/source)
          export BOOST_TEST_LOGGER=JUNIT:HRF
          ctest --verbose -j $NIX_BUILD_CORES --output-on-failure -R "${nixpkgs.lib.concatStringsSep "|" testList}" || true

          mkdir -p ${placeholder "out"}/test-logs
          find .. -type f -name '*_test.xml' -exec cp {} ${placeholder "out"}/test-logs \;
        '';

        checkInputs = defaultCheckInputs;

        GTEST_OUTPUT = "xml:${placeholder "out"}/test-reports/";

        dontInstall = true;
      });

      makeDevShell = pkgs.mkShell {
        nativeBuildInputs = defaultNativeBuildInputs
          ++ defaultBuildInputs
          ++ defaultCheckInputs
          ++ defaultDevTools;

        shellHook = ''
          echo "zkLLVM dev environment activated"
        '';
      };
    in
    {
      packages = {
        default = releaseBuild;
        debug = debugBuild;
      };
      checks = {
        release-tests = testBuild releaseBuild;
        debug-tests = testBuild debugBuild;
      };
      apps = {
        assigner = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/assigner";
        };
        clang = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/clang";
        };
        transpiler = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/transpiler";
        };
      };
      devShells.default = makeDevShell;
    }
    );
}

# To override some inputs:
# nix build --override-input nil-crypto3 /your/local/path/crypto3/
# to configure build:
# nix develop . -c cmake -B build -DCMAKE_CXX_STANDARD=17 -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=FALSE -DCMAKE_ENABLE_TESTS=TRUE
# to build:
# cd build
# nix develop ../ -c cmake --build . -t compile_cpp_examples

