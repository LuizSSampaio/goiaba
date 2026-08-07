logger_dep = dependency('goiaba_logger')
renderer_dep = dependency('goiaba_renderer')

sample_dep = declare_dependency(dependencies: [logger_dep, renderer_dep])

sample_exec = executable(
    meson.project_name(),
    'main.cpp',
    dependencies: sample_dep,
)
