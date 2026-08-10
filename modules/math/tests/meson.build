doctest_dep = dependency('doctest')
glm_dep = dependency('glm')

test_exe = executable(
    'test_math',
    [
        'test_main.cpp',
        'test_Vector.cpp',
        'test_Matrix.cpp',
        'test_Quaternion.cpp',
        'test_Functions.cpp',
    ],
    dependencies: [
        doctest_dep,
        math_dep,
        glm_dep,  # dev-only: enables differential tests against glm itself
    ],
)
test('math', test_exe)