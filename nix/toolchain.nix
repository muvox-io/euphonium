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
    "linux-amd64" = "0drd0pid0b5dc8j1ypbr78qqflp3sdq0z8q4wm0in0f21424xwcf";
    "linux-arm64" = "1dw9l485wl64q4dhhpy1mzcdqzkin907hr7q85m78b7m5sn1glp7";
    "linux-armel" = "12qgd7v6yhvcp9vrrv0knys1lcl40jmc1kn6gsqhvz4n8khjdfvg";
    "linux-armhf" = "04hv96gaarwhqa8n36pjx3mz1mqi27kxa6npmz3jljmhlidqhpdh";
    "linux-i686" = "1jvf58p96w7kd573dhh1qv8hzl0bmidq6ai1c4h7l80qn82y1y2x";
    "macos" = "1i2x1nsvk35v63kn74vczm6bgcgr0jhwlz4zk163ljh2vagi1bqr";
    "macos-arm64" = "07bb9g6a14bd5l02xh6g4p4c1zpjs0qpgzszi49niabh5mvqjhdi";
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