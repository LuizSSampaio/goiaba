doctest_dep = dependency('doctest')
spdlog_dep = dependency('spdlog')
logger_inc = include_directories('../include')

test_exe = executable(
    'test_logger',
    ['test_Logger.cpp', '../src/Logger.cpp'],
    dependencies: [doctest_dep, spdlog_dep],
    include_directories: logger_inc,
)
test('logger', test_exe)
