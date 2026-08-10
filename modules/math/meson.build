lib_name = meson.project_name() + '_' + 'math'

logger_dep = dependency('goiaba_logger')

glm_dep = dependency('glm')

math_lib_inc = include_directories('include')

# Private build dependencies: glm is linked here so the library's own .cpp
# files can use it, but it is intentionally NOT propagated to consumers (see
# `math_dep` below) — that keeps the glm dependency completely hidden behind
# the static-library boundary.
math_lib_deps = declare_dependency(
    dependencies: [
        logger_dep,
        glm_dep,
    ],
    include_directories: math_lib_inc,
)

math_lib = library(
    lib_name,
    [
        'src/GlmBridge.hpp',  # generate compile-commands / dependency tracking
        'src/Matrix.cpp',
        'src/Quaternion.cpp',
        'src/Functions.cpp',
    ],
    dependencies: math_lib_deps,
    # Right-handed view, Vulkan clip space (depth 0..1). Defined privately so
    # consumers never need to know about glm's compile config.
    cpp_args: ['-DGLM_FORCE_DEPTH_ZERO_TO_ONE'],
)

# Public dependency: links the static lib + headers, but carries NO glm dep.
math_dep = declare_dependency(
    link_with: math_lib,
    include_directories: math_lib_inc,
)

meson.override_dependency(lib_name, math_dep)

if get_option('tests').enabled()
    subdir('tests')
endif