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
        target-esp32 = (pkgs.callPackage ./nix/target-esp32.nix { });
        darwinPackages =
          if pkgs.stdenv.isDarwin then [
            pkgs.darwin.apple_sdk.frameworks.CoreServices
            pkgs.darwin.apple_sdk.frameworks.ApplicationServices
            pkgs.darwin.apple_sdk.frameworks.Security
          ] else
            [

            ];
      in
      {
        packages.target-esp32 = target-esp32;
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
                ncurses5
                target-esp32

                # Python, mostly for esp-idf
                (python3.withPackages (p: with p; [ pip virtualenv ]))

                # Build tool specific
                nodejs
                yarn
              ] ++ darwinPackages;

            shellHook = ''
              export ESP32=${target-esp32}
              export IDF_PATH=${target-esp32}/sdk
              export IDF_TOOLS_PATH=${target-esp32}/.espressif
              export PATH=$IDF_PATH/tools:$PATH

              export IDF_PYTHON_ENV_PATH=$IDF_TOOLS_PATH/python_env/idf5.0_py3.9_env
              . $IDF_PYTHON_ENV_PATH/bin/activate
            '';
          };
        };

      });
}
