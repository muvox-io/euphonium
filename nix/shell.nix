{}:

let
    pkgs = import(builtins.fetchTarball "channel:nixos-22.05") {};
    darwinPackages =  if pkgs.stdenv.isDarwin then [ pkgs.darwin.apple_sdk.frameworks.CoreServices pkgs.darwin.apple_sdk.frameworks.ApplicationServices pkgs.darwin.apple_sdk.frameworks.Security ] else [];
in
pkgs.mkShell {
    buildInputs = with pkgs; [
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

      # esp-idf specific
      (pkgs.callPackage ./esp-idf/toolchain.nix {})
      ncurses5

      # Python, mostly for esp-idf
      (python3.withPackages (p: with p; [
        pip
        virtualenv
      ]))

      # Build tool specific
      nodejs
      yarn
    ]
    ++ darwinPackages;
    shellHook = ''
      export IDF_PATH=$(pwd)/nix/esp-idf/sdk
      export PATH=$IDF_PATH/tools:$PATH
      export IDF_PYTHON_ENV_PATH=$(pwd)/nix/.python_env

      export EUPH_BUILD_CLI=$(pwd)/src/euph-build
      export PATH=$EUPH_BUILD_CLI/bin:$PATH

      if [ ! -e $IDF_PYTHON_ENV_PATH ]; then
        python -m venv $IDF_PYTHON_ENV_PATH
        . $IDF_PYTHON_ENV_PATH/bin/activate
        pip install -r $IDF_PATH/requirements.txt
        pip install -r $(pwd)/src/requirements.txt
        yarn --cwd $EUPH_BUILD_CLI
      else
        . $IDF_PYTHON_ENV_PATH/bin/activate
      fi

      echo 'Euphonium dev environment is now active'
      echo 'Access the dev-tool, by calling `euph-build --help`'

  '';
}