# Goiaba Engine

A modular C++23 game engine built on Vulkan. Currently early-stage, featuring a
Vulkan rendering backend, an abstract platform layer, a glm-backed math library,
and a spdlog-based logger.

> **Status:** Work in progress — the sample renders a triangle via SDL3 + Vulkan.

## Features

- **Renderer** — Vulkan 1.3 backend using Vulkan-Hpp (RAII) and VulkanMemoryAllocator.
  Backend selection is abstracted behind an `IBackend` interface.
- **Platform** — Window/Surface abstractions (`Window`, `Surface`,
  `SurfaceFactory`) decoupled from any specific windowing library.
- **Math** — Templated vectors (`TVec2/3/4`), matrices (`TMat`), and quaternions
  with right-handed conventions and Vulkan clip space (depth `0..1`). GLM is used
  internally but kept completely hidden behind the static-library boundary, so
  consumers have no GLM dependency.
- **Logger** — Thin static wrapper around spdlog with engine/game environment
  tagging.
- **Shaders** — [Slang](https://shader-slang.com/) shader language support.

## Tech Stack

| Area        | Technology                                   |
| ----------- | -------------------------------------------- |
| Language    | C++23                                        |
| Build       | Meson + Ninja                                |
| Windowing   | SDL3                                         |
| Graphics    | Vulkan 1.3 (Vulkan-Hpp RAII, VMA-Hpp)        |
| Math        | GLM (private)                                |
| Logging     | spdlog                                       |
| Testing     | doctest                                      |
| Docs        | Doxygen + Sphinx (Breathe, Exhale, Furo)     |
| Dev shell   | Nix flakes                                   |
| Tasks       | just                                         |

## Project Layout

```
goiaba/
├── meson.build            # Top-level Meson project
├── meson_options.txt      # Meson options (e.g. tests)
├── flake.nix              # Nix development shell
├── justfile               # Task runner recipes
├── docs/                  # Sphinx + Doxygen documentation
├── modules/               # Engine static libraries
│   ├── logger/            #   spdlog wrapper          (GE::Logger)
│   ├── math/              #   Math types & functions  (GE::Math)
│   ├── platform/          #   Window/Surface abstractions (GE::Platform)
│   └── renderer/          #   Vulkan renderer         (GE::Render)
├── samples/               # Sample application (triangle)
│   ├── main.cpp
│   └── shader.slang       # Slang source -> shader.spv
└── subprojects/           # Meson wraps (SDL3, glm, spdlog, doctest, ...)
```

Each module is built as a static library and exported as a Meson dependency
(e.g. `goiaba_renderer`, `goiaba_math`, `goiaba_logger`, `goiaba_platform`).

## Prerequisites

A Vulkan-capable GPU and driver are required. The recommended way to get a
complete, reproducible toolchain is via [Nix](https://nixos.org/):

```sh
# Enter the development shell (provides clang, meson, ninja, vulkan SDK,
# SDL3, shader-slang, doxygen, sphinx, just, and all runtime libraries)
nix develop
```

Without Nix, you will need: a C++23 compiler (clang ≥ 17 / GCC ≥ 14), Meson,
Ninja, pkg-config, and the development packages for SDL3, Vulkan, spdlog, GLM,
and VMA-Hpp.

## Building

Using [just](https://github.com/casey/just) (recommended):

```sh
just setup    # meson setup build -Dbuildtype=debugoptimized
just build    # meson compile -C build
just run      # ./build/samples/goiaba
just brun     # build + run in one step
```

Or with Meson directly:

```sh
meson setup build -Dbuildtype=debugoptimized
meson compile -C build
./build/samples/goiaba
```

## Tests

Tests use [doctest](https://github.com/doctest/doctest) and are gated behind the
`tests` Meson option (default `auto`):

```sh
meson setup build -Dtests=enabled
meson test -C build        # or: just test
```

## Documentation

API documentation is generated with Doxygen and rendered through Sphinx:

```sh
just docs        # run Doxygen then Sphinx
just docs-serve  # serve the HTML docs at http://localhost:8080
```

## Available `just` Recipes

| Recipe          | Description                                   |
| --------------- | --------------------------------------------- |
| `just setup`    | Configure the Meson build directory           |
| `just build`    | Compile the project                           |
| `just run`      | Run the sample binary                         |
| `just brun`     | Build, then run                               |
| `just test`     | Run the test suite                            |
| `just clear`    | Remove the `build/` directory                 |
| `just docs`     | Generate Doxygen + Sphinx documentation       |
| `just docs-serve` | Serve generated docs on `localhost:8080`    |

