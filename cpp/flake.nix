{
  description = "playground — C++ exercises alongside the cdb project (no CMake: one .cpp file = one program)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});
    in
    {
      # `nix develop` (or direnv) — the same g++ compiler as in cdb,
      # because it comes from the default nixpkgs stdenv.
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            gnumake
            clang-tools # clangd (LSP), clang-format, clang-tidy
          ] ++ pkgs.lib.optionals pkgs.stdenv.hostPlatform.isLinux [
            gdb
            valgrind
          ];

          # The "hardening" flags from the nixpkgs wrapper get in the way at -O0
          # and with sanitizers (see the comment in cdb/flake.nix).
          hardeningDisable = [ "all" ];

          shellHook = ''
            echo "playground — $(c++ --version | head -n1)"
            echo "  make run-00_hello        # compiles with sanitizers and runs"
            echo "  make help                # all commands"
          '';
        };
      });
    };
}
