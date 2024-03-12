{
  description = "euphonium - Tiny audio platform";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-23.11";
    nixpkgs-unstable.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    gk-flasher = {
      url = "github:muvox-io/gk-flasher";
      inputs.nixpkgs.follows = "nixpkgs-unstable";
    };
  };

  outputs = { self, nixpkgs, nixpkgs-unstable, flake-utils, gk-flasher }:
    let
      overlay = final: prev: {
        euphonium = (final.callPackage ./nix { });
        unstable = nixpkgs-unstable.legacyPackages.${prev.system};
        gk-flasher = gk-flasher.packages.${prev.system}.default;
      };
    in {
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
          app-esp32 = (pkgs.euphonium.app-esp32 {
            appVersion = self.rev or self.dirtyRev or "dirty";
          });
          euphoniumcli = pkgs.euphonium.euphoniumcli;
        };

        devShells = {
          esp32 = pkgs.euphonium.shell-esp32;
          frontend = pkgs.euphonium.shell-frontend;
        };

      in {
        inherit apps devShells packages;
        checks = packages;
        devShell = devShells.esp32;
      });
}
