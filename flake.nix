{
  inputs = {
    nixpkgs.url = "nixpkgs";
    flake-utils.url = "flake-utils";
    hfsutils.url = "github:hautot211/hfsutils/nix";
  };

  outputs = {self, nixpkgs, flake-utils, hfsutils, ...}:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        nativeBuildInputs = [
          hfsutils.packages.${system}.default
        ];
        buildInputs = [
          pkgs.gnumake
          pkgs.cmake
          pkgs.fuse3
        ];
        hfuse = pkgs.stdenv.mkDerivation {
          pname = "hfuse";
          version = "0.1.0";
          src = self;
          inherit buildInputs nativeBuildInputs;
        };
      in
    {
      packages.default = hfuse;
      devShells = {
        default = pkgs.mkShell {
          inherit buildInputs nativeBuildInputs;
          packages = [
            pkgs.valgrind
            pkgs.gdb
          ];
        };
        
        hfuse = pkgs.mkShell {
          import = [self.devShells.default];
          packages = [hfuse];
        };
      };
    }
  );
}
