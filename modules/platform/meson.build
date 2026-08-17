lib_name = meson.project_name() + '_' + 'platform'

logger_dep = dependency('goiaba_logger')

platform_lib_inc = include_directories('include')

platform_lib_deps = declare_dependency(
    dependencies: [
        logger_dep,
    ],
    include_directories: platform_lib_inc,
)

platform_lib = library(
    lib_name,
    [],
    dependencies: platform_lib_deps,
)

# if get_option('tests').enabled()
#     subdir('tests')
# endif

platform_dep = declare_dependency(
    link_with: platform_lib,
    include_directories: platform_lib_inc,
)

meson.override_dependency(lib_name, platform_dep)
