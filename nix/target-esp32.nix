{ stdenv, lib, fetchurl, fetchgit, makeWrapper, pkgs }:

let
  esp-idf = (pkgs.callPackage ./esp-idf.nix { });
  darwinPackages =
    if pkgs.stdenv.isDarwin then [
      pkgs.darwin.apple_sdk.frameworks.CoreServices
      pkgs.darwin.apple_sdk.frameworks.ApplicationServices
      pkgs.darwin.apple_sdk.frameworks.Security
    ] else [ ];
in

stdenv.mkDerivation rec {
  pname = "euphonium-target-esp32";
  version = "0.1.0";

  src = ../.;

  buildInputs = with pkgs;
    [
      # Build depds
      cmake
      ninja
      git
      wget
      gnumake
      flex
      bison
      gperf
      pkgconfig

      esp-idf
      # esp-idf specific
      ncurses5

      # Python, mostly for esp-idf
      (python3.withPackages (p: with p; [ pip virtualenv ]))

      # Build tool specific
      nodejs
      yarn
    ] ++ darwinPackages;

  postUnpackPhase = ''
    ls -al
  '';

  buildPhase = ''
    export HOME=$TMP
    export IDF_PATH=${esp-idf}/sdk
    export IDF_TOOLS_PATH=${esp-idf}/.espressif
    export PATH=$IDF_PATH/tools:$PATH

    export IDF_PYTHON_ENV_PATH=$IDF_TOOLS_PATH/python_env/idf5.0_py3.9_env
    . $IDF_PYTHON_ENV_PATH/bin/activate

    cd src/targets/esp32
    idf.py build
  '';

  installPhase = ''
    cd build

    mkdir -p $out $out/bootloader $out/partition_table
    cp storage.bin $out/storage.bin
    cp euphonium-esp32.bin $out/euphonium-esp32.bin
    cp bootloader/bootloader.bin $out/bootloader/bootloader.bin
    cp partition_table/partition-table.bin $out/partition_table/partition-table.bin
  '';

  dontConfigure = true;
  dontPatch = true;

  phases = [ "unpackPhase" "postUnpackPhase" "buildPhase" "installPhase" ];

  meta = with lib; {
    description = "Euphonium build for ESP32 platform";
    homepage = "https://github.com/muvox-io/euphonium";
    license = licenses.gpl3;
  };
}
