# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'Goiaba Engine'
copyright = '2026, Luiz Henrique Silva Sampaio'
author = 'Luiz Henrique Silva Sampaio'
release = 'master'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['breathe', 'exhale']

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

breathe_projects = {"Goiaba Engine": "../docs/xml"}
breathe_default_project = "Goiaba Engine"
breathe_implementation_filename_extensions = ['.cpp']

exhale_args = {
    'containmentFolder': './api',
    'rootFileName': 'library_root.rst',
    'rootFileTitle': 'Engine API',
    'doxygenStripFromPath': '..',
    'createTreeView': True,
    "contentsDirectives": False,
}

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'furo'
html_static_path = ['_static']
