default: build

builddir := 'build'
binName := 'goiaba'

build:
    meson compile -C {{ builddir }}

run:
    ./{{ builddir }}/samples/{{ binName }}

brun: build run

setup:
    meson setup {{ builddir }}

test:
    meson test -C {{ builddir }}

clear:
    rm -rf ./{{ builddir }}

docs-doxygen:
    cd docs && doxygen Doxyfile

docs-sphinx:
    cd docs && sphinx-build -b html . _build/html

docs: docs-doxygen docs-sphinx

docs-serve:
    python3 -m http.server 8080 --directory docs/_build/html
