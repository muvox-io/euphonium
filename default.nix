{ stdenv, callPackage }:

let
  frontend = callPackage ./src/frontend { };
in
{
  # Euphonium's webinterface
  frontend = frontend.static;

  # Build recipe for storage
  fs-esp32 = stdenv.mkDerivation {
    name = "euphonium-fs-esp32";
    src = ./src/core/fs;
    buildInputs = [ frontend.static ];
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
      cp -r ${frontend.static} $out/fs/pkgs/web/dist
    '';
  };
}
