logger_dep = dependency('goiaba_logger')
renderer_dep = dependency('goiaba_renderer')
sdl3_dep = dependency('sdl3')
vulkan_headers_dep = dependency('VulkanHeaders')

sample_dep = declare_dependency(dependencies: [logger_dep, renderer_dep])

sample_exec = executable(
    meson.project_name(),
    'main.cpp',
    dependencies: [sample_dep, sdl3_dep, vulkan_headers_dep],
)
