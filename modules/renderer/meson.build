lib_name = meson.project_name() + '_' + 'renderer'

logger_dep = dependency('goiaba_logger')

vk_loader_dep = dependency('vulkan')
vk_headers_dep = dependency('VulkanHeaders')
vk_vma_hpp_dep = dependency('VulkanMemoryAllocator-Hpp')
sdl3_dep = dependency('sdl3')

renderer_lib_inc = include_directories('include')

renderer_lib_deps = declare_dependency(
    dependencies: [
        logger_dep,
        vk_loader_dep,
        vk_headers_dep,
        vk_vma_hpp_dep,
        sdl3_dep,
    ],
    include_directories: renderer_lib_inc,
)

renderer_lib = library(
    lib_name,
    ['src/Renderer.cpp', 'src/backend/vulkan/Vulkan.cpp'],
    dependencies: renderer_lib_deps,
)

if get_option('tests').enabled()
    subdir('tests')
endif

renderer_dep = declare_dependency(
    link_with: renderer_lib,
    include_directories: renderer_lib_inc,
)

meson.override_dependency(lib_name, renderer_dep)
