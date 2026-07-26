spdlog_dep = dependency('spdlog')

logger_lib_inc = include_directories('include')

logger_lib_deps = declare_dependency(dependencies: [spdlog_dep], include_directories: logger_lib_inc,)

logger_lib = library(meson.project_name() + '_' + 'logger', 'src/Logger.cpp', dependencies: logger_lib_deps)

if get_option('tests').enabled()
    subdir('tests')
endif
