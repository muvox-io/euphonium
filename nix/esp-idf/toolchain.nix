{ stdenv, lib, fetchurl, makeWrapper, buildFHSUserEnv }:

let
  platforms = {
    "x86_64-linux" = "linux-amd64";
    "aarch64-linux" = "linux-arm64";
    "armv7l-unknown-linux" = "linux-armel";
    "armv7-unknown-linux" = "linux-armhf";
    "i686-linux" = "linux-i686";
    "x86_64-darwin" = "macos";
    "aarch64-darwin" = "macos-arm64";
    "i686-windows" = "win32";
    "x86_64-windows" = "win64";
  };
  platformHash = {
    "linux-amd64" = "161wxgfhln82788717m0wv9acvvp1mbh39i9m3fbn9hqqkp62873";
    "linux-arm64" = "04py5c277d563vf04934xcdjbyq6rcdwaz3fh31c6fzvmbfckghz";
    "linux-armel" = "12qgd7v6yhvcp9vrrv0knys1lcl40jmc1kn6gsqhvz4n8khjdfvg";
    "linux-armhf" = "04hv96gaarwhqa8n36pjx3mz1mqi27kxa6npmz3jljmhlidqhpdh";
    "linux-i686" = "1jvf58p96w7kd573dhh1qv8hzl0bmidq6ai1c4h7l80qn82y1y2x";
    "macos" = "0rkj8jgx8l0bvqqf1j1r4q5w6ccnwlb2ypaizl6j47kj0ch9y0m3";
    "macos-arm64" = "sUGJdy1wqWgTiV//dzHQ8v7AyCXPwC4ALW2RoMxLax0";
  };
  platform = platforms.${stdenv.system};
  toolchainHash = platformHash.${platform};
in

stdenv.mkDerivation rec {
  pname = "esp32-toolchain";
  version = "2021r2-patch5";

  src = fetchurl {
    url = "https://github.com/espressif/crosstool-NG/releases/download/esp-${version}/xtensa-esp32-elf-gcc8_4_0-esp-${version}-${platform}.tar.gz";
    sha256 = toolchainHash;
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