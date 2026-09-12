{
  description = "Raylib development environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self , nixpkgs ,... }: let
    system = "x86_64-linux";
  in {
    devShells."${system}".default = let
      pkgs = import nixpkgs {
        inherit system;
      };
    in pkgs.mkShell {
      packages = with pkgs; [
  pkg-config
  mesa
  libGL

  # Wayland
  wayland
  wayland-scanner
  wayland-protocols
  libxkbcommon

  # X11 / XWayland
  xorg.libX11
  xorg.libXrandr
  xorg.libXinerama
  xorg.libXcursor
  xorg.libXi

  cmake

  raylib
  glfw
  renderdoc
];      
LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath (with pkgs; [
  libGL

  wayland
  wayland-scanner
  wayland-protocols
  libxkbcommon

  xorg.libX11
  xorg.libXrandr
  xorg.libXinerama
  xorg.libXcursor
  xorg.libXi
]);
    };
  };
}
