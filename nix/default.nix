{ stdenv, pkgs }:

let
  frontend-pkg = (pkgs.callPackage ../src/frontend { });
  esp-idf = (pkgs.callPackage ./esp-idf.nix { });
in
rec {
  # Euphonium's webinterface
  frontend = frontend-pkg.static;

  # Build recipe for storage
  fs-esp32 = stdenv.mkDerivation {
    name = "euphonium-fs-esp32";
    src = ../src/core/fs;
    buildInputs = [ frontend ];
    installPhase = ''
      mkdir -p $out/fs/cfg $out/fs/pkgs $out/fs/tmp

      # copy packages
      cp -r pkgs/system $out/fs/pkgs/system
      cp -r pkgs/bluetooth $out/fs/pkgs/bluetooth
      cp -r pkgs/cspot $out/fs/pkgs/cspot
      cp -r pkgs/radio $out/fs/pkgs/radio

      # copy platform support
      cp -r pkgs/platform_esp32 $out/fs/pkgs/platform_esp32 

      # copy web-ui files
      mkdir -p $out/fs/pkgs/web
      cp -r ${frontend} $out/fs/pkgs/web/dist
    '';
  };

  # Esp32 devshell
  shell-esp32 = pkgs.mkShell {
    inputsFrom = [ esp-idf ];
    shellHook = ''
      export IDF_PATH=${esp-idf}/sdk
      export IDF_TOOLS_PATH=${esp-idf}/.espressif
      export PATH=$IDF_PATH/tools:$PATH
      export IDF_PYTHON_ENV_PATH=$IDF_TOOLS_PATH/python_env/idf5.0_py3.9_env 
    '';
  };

  # Build target esp32
  app-esp32 = stdenv.mkDerivation {
    name = "euphonium-esp32";
    src = ../.;
    nativeBuildInputs = [ esp-idf fs-esp32 ];
    buildPhase = ''
      runHook preBuild
      export HOME=$TMP
      cd src/targets/esp32

      # Include built fs
      rm -rf fs
      cp -r ${fs-esp32}/fs fs

      # build firmware
      idf.py build
      runHook postBuild
    '';
  };
}
