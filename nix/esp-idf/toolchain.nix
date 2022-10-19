{ stdenv, lib, fetchurl, makeWrapper, buildFHSUserEnv }:

let
  test = "";
in

stdenv.mkDerivation rec {
  pname = "esp32-toolchain";
  version = "2021r2-patch5";

  src = fetchurl {
    url = "https://github.com/espressif/crosstool-NG/releases/download/esp-${version}/xtensa-esp32-elf-gcc8_4_0-esp-${version}-macos-arm64.tar.gz";
    hash = "sha256-sUGJdy1wqWgTiV//dzHQ8v7AyCXPwC4ALW2RoMxLax0";
  };

  buildInputs = [ makeWrapper ];

  phases = [ "unpackPhase" "installPhase" ];

  installPhase = ''
    cp -r . $out
  '';

  meta = with lib; {
    description = "ESP32 toolchain";
    homepage = https://docs.espressif.com/projects/esp-idf/en/stable/get-started/linux-setup.html;
    license = licenses.gpl3;
  };
}