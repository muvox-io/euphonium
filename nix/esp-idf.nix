{ stdenv, lib, fetchFromGitHub, fetchurl, makeWrapper, pkgs }:

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
  pname = "esp-idf";
  version = "5.0";

  src = fetchFromGitHub {
    owner = "espressif";
    repo = "esp-idf";
    rev = "490216a2ace6dc3e1b9a3f50d265a80481b32f6d";
    fetchSubmodules = true;
    name = pname;
    sha256 = "KiLMD6q1SNO8Hl8ByulpWWCzjv2JWy6H/J1FE0W0UAY=";
  };
  
  toolchainSrc = fetchurl {
    url = "https://github.com/espressif/crosstool-NG/releases/download/esp-2022r1/xtensa-esp32-elf-gcc11_2_0-esp-2022r1-${platform}.tar.xz";
    sha256 = toolchainHash;
    name = "toolchain";
  };

  buildInputs = with pkgs;
    [
      (python3.withPackages (p: with p; [ pip virtualenv ]))
    ];

  unpackPhase = ''
    runHook preUnpack

    mkdir sdk
    cp -r $src/* sdk
    tar -xf $toolchainSrc

    runHook postUnpack
  '';

  installPhase = ''
    runHook preInstall
    
    mkdir -p $out/sdk $out/.espressif
    cp -r sdk/* $out/sdk
    echo "v5.0-494-g490216a2ac" >> $out/sdk/version.txt
    cp -r xtensa-esp32-elf/* $out 
    ls -al

    export IDF_PATH=$out/sdk
    export IDF_TOOLS_PATH=$out/.espressif
    export IDF_PYTHON_ENV_PATH=$IDF_TOOLS_PATH/python_env/idf5.0_py3.9_env

    touch $IDF_TOOLS_PATH/espidf.constraints.v5.0.txt
    python -m venv $IDF_PYTHON_ENV_PATH
    . $IDF_PYTHON_ENV_PATH/bin/activate
    pip install -r $IDF_PATH/tools/requirements/requirements.core.txt
    pip install protobuf grpcio-tools

    runHook postInstall
  '';

  phases = [ "unpackPhase" "installPhase" ];

  dontConfigure = true;

  dontBuild = true;

  meta = with lib; {
    description = "SDK for Espressif's chips";
    homepage = "https://github.com/espressif/esp-idf";
    license = licenses.gpl3;
  };
}
