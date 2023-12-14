# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
import subprocess
from pathlib import Path


sys.path.insert(0, os.path.abspath("../python"))

# are we running on readthedocs?
on_readthedocs = os.environ.get("READTHEDOCS", False) == "True"

doc_dir = Path(__file__).parent

# -- Project information -----------------------------------------------------

project = "PODIO"
copyright = "2023, Key4hep authors"
author = "Key4hep authors"


# The full version, including alpha/beta/rc tags
# version = '@podio_VERSION@'
# release = version


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "myst_parser",
    "breathe",
    # auto generation of python bindings documentation from docstrings
    "sphinx.ext.napoleon",
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ["_templates"]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = "sphinx_rtd_theme"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ["_static"]


# Make sure that the __init__ docstrings appear as part of the class
# documentation
autoclass_content = "both"

# -- Doxygen integration with Breathe -----------------------------------------


# -- Automatic API documentation ----------------------------------------------

print(f"Executing doxygen in {doc_dir}")
doxygen_version = subprocess.check_output(["doxygen", "--version"], encoding="utf-8")
print(f"Doxygen version: {doxygen_version}")

env = os.environ.copy()
env["DOXYGEN_WARN_AS_ERROR"] = "NO"

os.makedirs("_build/cpp", exist_ok=True)

subprocess.check_call(
    ["doxygen", "Doxyfile"], stdout=subprocess.PIPE, cwd=doc_dir, env=env
)

api_index_targetdir = doc_dir / "cpp_api/api.md"

print(f"Executing breath apidoc in {doc_dir}")
subprocess.check_call(
    [sys.executable, "-m", "breathe.apidoc", "_build/cpp/doxygen-xml", "-o", "cpp_api"],
    stdout=subprocess.PIPE,
    cwd=doc_dir,
    env=env,
)
