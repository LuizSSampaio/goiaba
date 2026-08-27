qt6 = import('qt6')
qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets'])

vulkan_headers_dep = dependency('VulkanHeaders')

logger_dep = dependency('goiaba_logger')
renderer_editor_dep = dependency('goiaba_renderer_editor')

moc_files = qt6.preprocess(moc_headers: ['src/Widgets/Viewport/Viewport.hpp'])

executable(
    meson.project_name(),
    [
        'src/main.cpp',
        'src/Widgets/Viewport/Viewport.cpp',
        'src/Widgets/Viewport/QtWindow.cpp',
        'src/Widgets/Viewport/QtSurfaceFactory.cpp',
    ] + moc_files,
    dependencies: [
        qt6_dep,
        vulkan_headers_dep,
        logger_dep,
        renderer_editor_dep,
    ],
)
