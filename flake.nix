{
  description = "Nix flake for zkLLVM";

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
        # Repo dependencies
        crypto3
        blueprint
      ];

      defaultDevTools = [
        pkgs.clang_17 # clang-format and clang-tidy
      ];


      defaultCmakeFlags = [
        "-DCMAKE_CXX_STANDARD=17"
        "-DBUILD_SHARED_LIBS=TRUE"
        "-DZKLLVM_VERSION=0.1.18"
      ];

      releaseBuild = stdenv.mkDerivation {
        name = "zkLLVM";
        cmakeBuildType = "Release";
        buildInputs = defaultBuildInputs ++ defaultNativeBuildInputs;

        ninjaFlags = "assigner clang transpiler";

        src = self; # Here we should ignore all tests/* test/* examples/* folders to minimize rebuilds

        doCheck = false;
        dontInstall = true;
      };

      # TODO: we need to propagate debug mode to dependencies here:
      debugBuild = releaseBuild.overrideAttrs (finalAttrs: previousAttrs: {
        name = previousAttrs.name + "-debug";
        cmakeBuildType = "Debug";
        buildInputs = defaultBuildInputs ++ defaultNativeBuildInputs;
      });

      makeDevShell = pkgs.mkShell {
        nativeBuildInputs = defaultNativeBuildInputs
          ++ defaultBuildInputs
          ++ defaultDevTools;

        shellHook = ''
          echo "zkLLVM dev environment activated"
          export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${placeholder "out"}/libs/circifier/llvm/lib"
        '';
      };
    in
    {
      packages = {
        default = releaseBuild;
        debug = debugBuild;
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
