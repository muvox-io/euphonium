{ stdenv, callPackage, nodejs, nodePackages, writeShellScriptBin }:

let
  # Import & invoke the generated files from node2nix
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
  # Location of the node_modules system dependencies
  inherit (generated) nodeDependencies;

  # Build recipe for the static assets
  static = stdenv.mkDerivation {
    name = "euphonium-frontend";
    src = ./.;
    buildInputs = [ nodejs ];
    buildPhase = ''
      export HOME=$(pwd)
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
