lib_name = meson.project_name() + '_' + 'renderer'

logger_dep = dependency('goiaba_logger')
math_dep = dependency('goiaba_math')
platform_dep = dependency('goiaba_platform')

vk_loader_dep = dependency('vulkan')
vk_headers_dep = dependency('VulkanHeaders')
vk_vma_hpp_dep = dependency('VulkanMemoryAllocator-Hpp')

renderer_lib_inc = include_directories('include')

renderer_lib_deps = declare_dependency(
    dependencies: [
        logger_dep,
        math_dep,
        platform_dep,
        vk_loader_dep,
        vk_headers_dep,
        vk_vma_hpp_dep,
    ],
    include_directories: renderer_lib_inc,
)

renderer_lib = library(
    lib_name,
    [
        'src/Renderer.cpp',
        'src/backends/Vulkan/Vulkan.cpp',
        'src/Shader.cpp',
    ],
    dependencies: renderer_lib_deps,
)

renderer_editor_lib = library(
    lib_name + '_editor',
    [
        'src/Renderer.cpp',
        'src/backends/Vulkan/Vulkan.cpp',
        'src/Shader.cpp',
    ],
    dependencies: renderer_lib_deps,
    cpp_args: ['-DGE_VULKAN_HOST_OWNED'],
)

if get_option('tests').enabled()
    subdir('tests')
endif

renderer_dep = declare_dependency(
    link_with: renderer_lib,
    dependencies: [
        platform_dep,
    ],
    include_directories: renderer_lib_inc,
)

meson.override_dependency(lib_name, renderer_dep)

renderer_editor_dep = declare_dependency(
    link_with: renderer_editor_lib,
    dependencies: [
        platform_dep,
    ],
    include_directories: renderer_lib_inc,
)

meson.override_dependency(lib_name + '_editor', renderer_editor_dep)
