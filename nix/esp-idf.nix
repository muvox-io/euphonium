{ stdenv
, lib
, fetchFromGitHub
, python39Packages
, fetchurl
, pkgs
,
}:
let
  idf-component-manager = python39Packages.buildPythonPackage rec {
    pname = "idf_component_manager";
    version = "1.2.2";
    src = python39Packages.fetchPypi {
      inherit pname version;
      sha256 = "sha256-NnLz4zpNERAFsaeuFhKs0RnYUdIU+uqwVOi7EPY4ByA=";
    };
    doCheck = false;
    configurePhase = ''
      export HOME=$TMP
    '';
    propagatedBuildInputs = with python39Packages; [
      packaging
      lockfile
      cachecontrol
      colorama
      click
      future
      pyyaml
      requests
      requests-file
      requests-toolbelt
      schema
      six
      tqdm
    ];
  };
  esptool = python39Packages.buildPythonPackage rec {
    pname = "esptool";
    version = "4.6.2";
    src = python39Packages.fetchPypi {
      inherit pname version;
      sha256 = "sha256-VJ75Pu9C7n6UYs5aU8Ft96DHHZGz934Z7BV0mATN8wA=";
    };
    doCheck = false;
    propagatedBuildInputs = with python39Packages; [
      bitstring
      cryptography
      ecdsa
      pyserial
      reedsolo
    ];
  };
  esp-coredump = python39Packages.buildPythonPackage rec {
    pname = "esp-coredump";
    version = "1.4.2";
    src = python39Packages.fetchPypi {
      inherit pname version;
      sha256 = "sha256-sAZQOxgm/kzj4+XaO6UvvtZMr89eP3FER8jkSwDLkvM=";
    };
    doCheck = false;
    propagatedBuildInputs = with python310Packages; [
      esptool
      pygdbmi
      construct
      pyyaml
    ];
  };
  esp-idf-kconfig = python39Packages.buildPythonPackage rec {
    pname = "esp-idf-kconfig";
    version = "1.1.0";
    src = python39Packages.fetchPypi {
      inherit pname version;
      sha256 = "sha256-s8ZXt6cf5w2pZSxQNIs/SODAUvHNgxyQ+onaCa7UbFA=";
    };
    doCheck = false;
    propagatedBuildInputs = with python39Packages; [
      kconfiglib
    ];
  };
  esp-idf-monitor = python39Packages.buildPythonPackage rec {
    pname = "esp-idf-monitor";
    version = "1.0.0";
    src = python39Packages.fetchPypi {
      inherit pname version;
      sha256 = "sha256-CrdQI4Lp7TQvmolf3aHuokrQlnrPneemMisG9TGcTls=";
    };
    doCheck = false;
    propagatedBuildInputs = with python39Packages; [
      pyserial
      pyelftools
      esp-coredump
    ];
  };
  freertos-gdb = python39Packages.buildPythonPackage rec {
    pname = "freertos-gdb";
    version = "1.0.2";
    src = python39Packages.fetchPypi {
      inherit pname version;
      sha256 = "sha256-o0ZoTy7OLVnrhSepya+MwaILgJSojs2hfmI86D9C3cs=";
    };
    doCheck = false;
    propagatedBuildInputs = with python39Packages; [
      setuptools
      wheel
    ];
  };
  toolchain = pkgs.callPackage ./toolchain.nix { };
  idf-python-env = (pkgs.python39.withPackages (p: with p; [ pip idf-component-manager esptool esp-coredump esp-idf-monitor esp-idf-kconfig freertos-gdb p.protobuf grpcio-tools ]));
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

  nativeBuildInputs = [ pkgs.makeWrapper ];

  propagatedBuildInputs = with pkgs; [
    which
    toolchain
    git
    cmake
    ninja
    protobuf
    idf-python-env
  ];

  unpackPhase = ''
    runHook preUnpack
    mkdir sdk
    cp -r $src/* sdk
    runHook postUnpack
  '';

  preBuildPhase = ''
    export HOME=$TMP
  '';

  installPhase = ''
    runHook preInstall

    # Fake venv, idf actually checks this
    mkdir -p $out/sdk $out/.espressif/python_env/idf5.0_py3.9_env/bin
    ln -s $(readlink -e $(which python3)) $out/.espressif/python_env/idf5.0_py3.9_env/bin/python
    touch $out/.espressif/espidf.constraints.v5.0.txt
    cp -r sdk/* $out/sdk

    # no .git, inject version
    echo "v5.0-494-g490216a2ac" >> $out/sdk/version.txt

    # prepare wrapper, that includes all idf-related env
    makeWrapper $out/.espressif/python_env/idf5.0_py3.9_env/bin/python $out/bin/idf.py \
    --add-flags $out/sdk/tools/idf.py \
    --set IDF_TOOLS_PATH $out/.espressif \
    --set IDF_PYTHON_ENV_PATH $out/.espressif/python_env/idf5.0_py3.9_env \
    --set IDF_PATH $out/sdk/ \
    --set PYTHONPATH ${idf-python-env}/${idf-python-env.sitePackages}:$PYTHONPATH \
    --prefix PATH : "${lib.makeBinPath propagatedBuildInputs}"

    # wrapper around esptool.py
    makeWrapper $out/.espressif/python_env/idf5.0_py3.9_env/bin/python $out/bin/esptool.py \
    --add-flags $out/sdk/components/esptool_py/esptool/esptool.py \
    --set IDF_TOOLS_PATH $out/.espressif \
    --set IDF_PYTHON_ENV_PATH $out/.espressif/python_env/idf5.0_py3.9_env \
    --set IDF_PATH $out/sdk/ \
    --prefix PATH : "${lib.makeBinPath propagatedBuildInputs}"
    runHook postInstall
  '';

  shellHook = ''
    export PYTHONPATH=${idf-python-env}/${idf-python-env.sitePackages}:$PYTHONPATH
    export PATH="${lib.makeBinPath propagatedBuildInputs}":$PATH
  '';

  phases = [ "unpackPhase" "preBuildPhase" "installPhase" ];

  dontConfigure = true;

  meta = with lib; {
    description = "SDK for Espressif's chips";
    homepage = "https://github.com/espressif/esp-idf";
    license = licenses.gpl3;
  };
}

