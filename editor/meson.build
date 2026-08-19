qt6 = import('qt6')
qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets'])

renderer_dep = dependency('goiaba_renderer')

moc_files = qt6.preprocess(moc_headers: ['src/Widgets/Viewport/Viewport.hpp'])

executable(
    meson.project_name(),
    [
        'src/main.cpp',
        'src/Widgets/Viewport/Viewport.cpp',
        'src/Widgets/Viewport/QtWindow.cpp',
    ] + moc_files,
    dependencies: [
        qt6_dep,
        renderer_dep,
    ],
)
