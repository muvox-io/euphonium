{
  description = "euphonium - Tiny audio platform";

  inputs.nixpkgs.url = "nixpkgs/nixos-22.11";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    let
      overlay = final: prev: {
        euphonium = (final.callPackage ./. { } // {
        });
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
        };

        packages = {
          frontend = pkgs.euphonium.frontend;
          fs-esp32 = pkgs.euphonium.fs-esp32;
        };

        devShells = {
        };

      in
      {
        inherit apps devShells packages;
        checks = packages;
      });
}
