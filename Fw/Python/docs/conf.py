"""Configuration file for the Sphinx documentation builder.

Documentation:
https://www.sphinx-doc.org/en/master/usage/configuration.html
"""
# pylint: disable=invalid-name

import re
from datetime import datetime
from importlib import import_module
from pathlib import Path

import sphinx_rtd_theme  # pylint: disable=unused-import


# importlib.metadata is implemented in Python 3.8
# Previous versions require the backport, https://pypi.org/project/importlib-metadata/
try:
    metadata = import_module("importlib.metadata")
except ModuleNotFoundError:
    metadata = import_module("importlib_metadata")

# -- Project information -----------------------------------------------------

metadata_ = metadata.metadata("fprime")
project = metadata_.get("Name")
author = metadata_.get("Author")
copyright = f"{datetime.now().year}, {author}"  # pylint: disable=redefined-builtin

# The full version, including alpha/beta/rc tags
release = metadata_.get("Version")
# The short X.Y version
version = re.match(r"v?\d+(\.\d+)*", release)[0]


# -- General configuration ---------------------------------------------------

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.autosectionlabel",
    "sphinx.ext.doctest",
    "sphinx.ext.coverage",
    "sphinx.ext.extlinks",
    "sphinx.ext.ifconfig",
    "sphinx.ext.intersphinx",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
    "autoapi.extension",
    "recommonmark",
    "sphinx_rtd_theme",
    "sphinxcontrib.mermaid",
]

source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}

master_doc = "index"

# templates_path = ["_templates"]
# exclude_patterns = []

rst_epilog = "\n".join(
    [
        "\nBuild: |release|\n",
        f".. _fprime: {metadata_.get('url')}",
        f".. |project| replace:: {project.replace('_', ' ').title()}",
    ]
)

nitpicky = True

# -- Options for HTML output -------------------------------------------------

html_theme = "sphinx_rtd_theme"
html_show_sphinx = False
html_static_path = ["_static"]
html_css_files = [
    str(Path("css", "rtd_width.css")),
]
# html_logo = None
# html_favicon = None

# -- Extension configuration -------------------------------------------------

todo_include_todos = True
autosectionlabel_prefix_document = True
autodoc_default_options = {
    "members": None,
    "undoc-members": True,
    "inherited-members": True,
    "autodoc_typehints": "description",
}
autoapi_type = "python"
autoapi_dirs = [str(Path(__file__).parents[1].joinpath("src"))]
autoapi_root = "api"
mermaid_version = "8.7.0"
