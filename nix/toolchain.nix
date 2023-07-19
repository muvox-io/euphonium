{ stdenv, lib, buildFHSUserEnv, makeWrapper, fetchurl }:

let
  platforms = {
    # nix name -> espressifs release name
    "x86_64-linux" = "x86_64-linux-gnu";
    "aarch64-linux" = "aarch64-linux-gnu";
    "armv7l-unknown-linux" = "arm-linux-gnueabi";
    "armv7-unknown-linux" = "arm-linux-gnueabihf";
    "i686-linux" = "i686-linux-gnu";
    "x86_64-darwin" = "x86_64-apple-darwin";
    "aarch64-darwin" = "aarch64-apple-darwin";
    "i686-windows" = "win32";
    "x86_64-windows" = "win64";
  };
  fhsEnv = if stdenv.isDarwin then {} else buildFHSUserEnv {
    name = "esp32-toolchain-env";
    targetPkgs = pkgs: with pkgs; [ zlib ];
    runScript = "";
  };
  platformHash = {
    # espressif release name -> sha256 hash
    "x86_64-linux-gnu" = "sha256-TS4C70fxqTpNz9uuzUhq36q0wOJt7qLBjWOFUn85+GQ=";
    "aarch64-linux-gnu" = "sha256-niEaGCtuoDlqQcePUvUdlk54df4nTqnIERG/DbyQxRY=";
    "arm-linux-gnueabi" = "sha256-Ld2R+5i3mzAEK3kY7vYM8Qx71bHahT6Dtl8pO5beyAA=";
    "arm-linux-gnueabihf" = "sha256-poOkaFVdy8ts4yoZCEIRDW+FPU1hBNYc8Lyd1Qxr4eY=";
    "i686-linux-gnu" = "sha256-KSsZ6mGGUIqSP7b9AQOXfgAdTrjneDbH49bObl+n0wU=";
    "x86_64-apple-darwin" = "sha256-sJ2H/bHcMs0dcYk1Bl75MbEBoU32sXvlZ0jlJkCVW/8=";
    "aarch64-apple-darwin" = "sha256-9QrKsrIW6UddxTE7PktCTLxw0KvSO6GBiv9KAZFl2o4=";
  };
  platform = platforms.${stdenv.system};
  toolchainHash = platformHash.${platform};
in

stdenv.mkDerivation rec {
  pname = "esp32-toolchain";
  version = "12.2.0_20230208";

  src = fetchurl {
    url = "https://github.com/espressif/crosstool-NG/releases/download/esp-${version}/xtensa-esp32-elf-${version}-${platform}.tar.xz";
    sha256 = toolchainHash;
  };

  buildInputs = [ makeWrapper ];

  phases = [ "unpackPhase" "installPhase" ];

  # FHS env not supported on darwin, although nix darwin is inherently impure so it does not matter
  installPhase = if stdenv.isDarwin then ''
    cp -r . $out
  '' else ''
    cp -r . $out
    for FILE in $(ls $out/bin); do
      FILE_PATH="$out/bin/$FILE"
      if [[ -x $FILE_PATH ]]; then
        mv $FILE_PATH $FILE_PATH-unwrapped
        makeWrapper ${fhsEnv}/bin/esp32-toolchain-env $FILE_PATH --add-flags "$FILE_PATH-unwrapped"
      fi
    done
  '';

  meta = with lib; {
    description = "ESP32 toolchain";
    homepage = https://docs.espressif.com/projects/esp-idf/en/stable/get-started/linux-setup.html;
    license = licenses.gpl3;
  };
}
