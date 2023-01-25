{ stdenv, lib, fetchurl, makeWrapper }:

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
    "linux-amd64" = "0c6z97x9lq5q5x181473zc27kf80i2vgs6kxxf7x2xc2w43q93b9";
    "linux-arm64" = "1dw9l485wl64q4dhhpy1mzcdqzkin907hr7q85m78b7m5sn1glp7";
    "linux-armel" = "12qgd7v6yhvcp9vrrv0knys1lcl40jmc1kn6gsqhvz4n8khjdfvg";
    "linux-armhf" = "04hv96gaarwhqa8n36pjx3mz1mqi27kxa6npmz3jljmhlidqhpdh";
    "linux-i686" = "1jvf58p96w7kd573dhh1qv8hzl0bmidq6ai1c4h7l80qn82y1y2x";
    "macos" = "1i2x1nsvk35v63kn74vczm6bgcgr0jhwlz4zk163ljh2vagi1bqr";
    "macos-arm64" = "0fvw5b9myjj1z298q2j51in9j94qnrhdk1addi4zs1skvjq4jwi9";
  };
  platform = platforms.${stdenv.system};
  toolchainHash = platformHash.${platform};
in

stdenv.mkDerivation rec {
  pname = "esp32-toolchain";
  version = "2022r1";

  src = fetchurl {
    url = "https://github.com/espressif/crosstool-NG/releases/download/esp-${version}/xtensa-esp32-elf-gcc11_2_0-esp-${version}-${platform}.tar.xz";
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
