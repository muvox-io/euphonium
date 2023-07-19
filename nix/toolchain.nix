{ stdenv, lib, buildFHSUserEnv, makeWrapper, fetchurl }:

let
  platforms = {
    "x86_64-linux" = "x86_64-linux-gnu";
    "aarch64-linux" = "linux-arm64";
    "armv7l-unknown-linux" = "linux-armel";
    "armv7-unknown-linux" = "linux-armhf";
    "i686-linux" = "linux-i686";
    "x86_64-darwin" = "macos";
    "aarch64-darwin" = "macos-arm64";
    "i686-windows" = "win32";
    "x86_64-windows" = "win64";
  };
  fhsEnv = if stdenv.isDarwin then {} else buildFHSUserEnv {
    name = "esp32-toolchain-env";
    targetPkgs = pkgs: with pkgs; [ zlib ];
    runScript = "";
  };
  platformHash = {
    "x86_64-linux-gnu" = "sha256-TS4C70fxqTpNz9uuzUhq36q0wOJt7qLBjWOFUn85+GQ=";
    "linux-arm64" = "1dw9l485wl64q4dhhpy1mzcdqzkin907hr7q85m78b7m5sn1glp7";
    "linux-armel" = "12qgd7v6yhvcp9vrrv0knys1lcl40jmc1kn6gsqhvz4n8khjdfvg";
    "linux-armhf" = "04hv96gaarwhqa8n36pjx3mz1mqi27kxa6npmz3jljmhlidqhpdh";
    "linux-i686" = "1jvf58p96w7kd573dhh1qv8hzl0bmidq6ai1c4h7l80qn82y1y2x";
    "macos" = "1gq9ai0pf5cvacg9kw6l0nk08s6k01r4n8g4q6z3m7j6aqy8g78w";
    "macos-arm64" = "0fvw5b9myjj1z298q2j51in9j94qnrhdk1addi4zs1skvjq4jwi9";
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
