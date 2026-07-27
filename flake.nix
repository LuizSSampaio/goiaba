{
  description = "A simple game engine project";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {inherit system;};
      in {
        devShells.default = let
          runtimeLibs = with pkgs; [
            # Audio backends
            alsa-lib
            pipewire
            pulseaudio

            # X11 stack (development headers)
            libX11
            libXext
            libXrandr
            libXcursor
            libXi
            libXfixes
            libXScrnSaver
            libXtst

            # XCB (for Vulkan/X11)
            libxcb

            # Wayland stack
            wayland
            wayland-protocols
            wayland-scanner
            libxkbcommon

            # GL / GPU
            libGL
            mesa

            # HID / USB (for SDL_HIDAPI_LIBUSB)
            libusb1

            dbus

            vulkan-validation-layers
          ];
        in
          pkgs.mkShell {
            nativeBuildInputs = with pkgs; [
              cmake
              meson
              mesonlsp
              clang
              lldb
              pkg-config
              ninja
              conan
              just
              doxygen
              python3
              python3Packages.pip

              vulkan-loader
              vulkan-tools
              vulkan-tools-lunarg
            ];

            buildInputs = runtimeLibs;

            shellHook = ''
              export LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath runtimeLibs}:$LD_LIBRARY_PATH"

              # Documentation virtual environment
              if [ ! -d .venv ]; then
                echo "Creating Python venv for docs..."
                python3 -m venv .venv
                .venv/bin/pip install --quiet \
                sphinx \
                breathe \
                exhale \
                furo
              fi

              export PATH="$PWD/.venv/bin:$PATH"
            '';
          };
      }
    );
}
