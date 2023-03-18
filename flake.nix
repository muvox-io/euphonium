{
  description = "euphonium - Tiny audio platform";

  inputs.nixpkgs.url = "nixpkgs/nixos-22.11";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    let
      overlay = final: prev: {
        euphonium = (final.callPackage ./nix { });
      };
    in
    {
      overlays.default = overlay;
    } // flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ overlay ];
        };

        apps = {
          flash-storage = {
            type = "app";
            program = "${pkgs.euphonium.flash-storage}/bin/flash-storage";
          };
        };

        packages = {
          frontend = pkgs.euphonium.frontend;
          fs-esp32 = pkgs.euphonium.fs-esp32;
          app-esp32 = pkgs.euphonium.app-esp32;
        };

        devShells = {
          esp32 = pkgs.euphonium.shell-esp32;
          frontend = pkgs.euphonium.shell-frontend;
        };

      in
      {
        inherit apps devShells packages;
        checks = packages;
        devShell = devShells.esp32;
      });
}
