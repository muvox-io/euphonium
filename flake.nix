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
        esp-idf = (pkgs.callPackage ./nix/esp-idf.nix { });
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
                esp-idf

                # Python, mostly for esp-idf
                (python3.withPackages (p: with p; [ pip virtualenv ]))

                # Build tool specific
                nodejs
                yarn
              ] ++ darwinPackages;

            shellHook = ''
              export IDF_PATH=${esp-idf}/sdk
              export IDF_TOOLS_PATH=$(pwd)/nix/.espressif
              export PATH=$IDF_PATH/tools:$PATH
              export IDF_PYTHON_ENV_PATH=$IDF_TOOLS_PATH/python_env/idf5.0_py3.9_env
              mkdir -p $IDF_TOOLS_PATH
              if [ ! -e $IDF_PYTHON_ENV_PATH ]; then
                touch $IDF_TOOLS_PATH/espidf.constraints.v5.0.txt
                python -m venv $IDF_PYTHON_ENV_PATH
                . $IDF_PYTHON_ENV_PATH/bin/activate
                pip install -r $IDF_PATH/tools/requirements/requirements.core.txt
                pip install protobuf grpcio-tools
                $IDF_PATH/tools/idf_tools.py install
              else
                . $IDF_PYTHON_ENV_PATH/bin/activate
              fi
              idf_exports=$("$IDF_PATH/tools/idf_tools.py" export)
              eval "$idf_exports"
            '';
          };
        };

      });
}
