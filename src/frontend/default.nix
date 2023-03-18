{ stdenv, callPackage, nodejs, nodePackages, writeShellScriptBin }:

let
  generated = callPackage ./nix { inherit nodejs; };

  # node2nix wrapper to update nix files on npm changes
  node2nix = writeShellScriptBin "node2nix" ''
    ${nodePackages.node2nix}/bin/node2nix \
      --development \
      -l package-lock.json \
      -c ./nix/default.nix \
      -o ./nix/node-packages.nix \
      -e ./nix/node-env.nix
  '';

in {
  inherit (generated) nodeDependencies;

  # Build recipe for the static assets
  static = stdenv.mkDerivation {
    name = "euphonium-frontend";
    src = ./.;
    buildInputs = [ nodejs ];
    buildPhase = ''
      export HOME=$TMP
      ln -s ${generated.nodeDependencies}/lib/node_modules ./node_modules
      export PATH="${generated.nodeDependencies}/bin:$PATH"
      npm run build
    '';
    installPhase = ''
      cp -r dist $out/
    '';
  };

  # Development shell with node2nix wrapper script
  shell = generated.shell.override {
    buildInputs = [ node2nix ];
  };
}
