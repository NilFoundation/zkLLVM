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

  outputs = { self
    , nixpkgs
    , flake-utils
    , nix-3rdparty
    , nil-crypto3
    , nil-zkllvm-blueprint
    }:
    (flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ nix-3rdparty.overlays.${system}.default ];
        };
        crypto3 = nil-crypto3.packages.${system}.default;
        blueprint = nil-zkllvm-blueprint.packages.${system}.default;

      in {
        packages = rec {
          zkllvm = (pkgs.callPackage ./zkllvm.nix { 
            src_repo = self;
            pkgs=pkgs;
            crypto3 = crypto3;
            blueprint = blueprint;
          });
          debug = (pkgs.callPackage ./zkllvm.nix {
            src_repo = self;
            pkgs=pkgs;
            enableDebug = true;
            crypto3 = crypto3;
            blueprint = blueprint;
          });
          default = zkllvm;
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
      }));
}

# To override some inputs:
# nix build --override-input nil-crypto3 /your/local/path/crypto3/
# to configure build:
# nix develop . -c cmake -B build -DCMAKE_CXX_STANDARD=17 -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=FALSE -DCMAKE_ENABLE_TESTS=TRUE
# to build:
# cd build
# nix develop ../ -c cmake --build . -t compile_cpp_examples
