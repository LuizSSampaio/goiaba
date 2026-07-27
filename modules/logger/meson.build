lib_name = meson.project_name() + '_' + 'logger'

spdlog_dep = dependency('spdlog')

logger_lib_inc = include_directories('include')

logger_lib_deps = declare_dependency(
    dependencies: [spdlog_dep],
    include_directories: logger_lib_inc,
)

logger_lib = library(
    lib_name,
    'src/Logger.cpp',
    dependencies: logger_lib_deps,
)

if get_option('tests').enabled()
    subdir('tests')
endif

logger_dep = declare_dependency(
    link_with: logger_lib,
    include_directories: logger_lib_inc,
)

meson.override_dependency(lib_name, logger_dep)
