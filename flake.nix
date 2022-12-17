{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-22.05";
    nixpkgs-unstable.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils, nixpkgs-unstable }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
        pkgs-unstable = import nixpkgs-unstable {
          inherit system;
        };
        darwinPackages = if pkgs.stdenv.isDarwin then [
          pkgs.darwin.apple_sdk.frameworks.CoreServices
          pkgs.darwin.apple_sdk.frameworks.ApplicationServices
          pkgs.darwin.apple_sdk.frameworks.Security
        ] else
          [

          ];
      in {
        devShells = {
          default = pkgs.mkShell {
            buildInputs = with pkgs;
              [
                # Build depds
                clang
                cmake
                ninja
                git
                wget
                gnumake
                flex
                bison
                gperf
                pkgconfig

                # Native dependencies
                portaudio
                mbedtls
                protobuf
                fmt

                # esp-idf specific
                (pkgs.callPackage ./nix/toolchain.nix { })
                ncurses5

                # Python, mostly for esp-idf
                (python3.withPackages (p: with p; [ pip virtualenv ]))

                # Build tool specific
                nodejs
                yarn
              ] ++ darwinPackages;
            shellHook =
              ''
                export IDF_PATH=$(pwd)/nix/esp-idf
                export PATH=$IDF_PATH/tools:$PATH
                export IDF_PYTHON_ENV_PATH=$(pwd)/nix/.python_env
                export EUPH_BUILD_CLI=$(pwd)/src/euph-build
                export PATH=$EUPH_BUILD_CLI/bin:$PATH

                if [ ! -e $IDF_PYTHON_ENV_PATH ]; then
                  python -m venv $IDF_PYTHON_ENV_PATH
                  . $IDF_PYTHON_ENV_PATH/bin/activate

                  pip install -r $IDF_PATH/requirements.txt
                  pip install -r $(pwd)/src/requirements.txt
                  pip install -r $(pwd)/docs/requirements.txt

                  yarn --cwd $EUPH_BUILD_CLI
                else
                . $IDF_PYTHON_ENV_PATH/bin/activate
                fi
                
                echo 'Euphonium dev environment is now active'
                echo 'Access the dev-tool, by calling `euph-build --help`'
              '';
          };
        };
      });
}
