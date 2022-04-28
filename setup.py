#!/usr/bin/env python
####
# fprime Python Package:
#
# A meta-package that pulls in the other fprime packages (fprime-fpp, fprime-tools, fprime-gds) into the user's
# environment. This makes installing a tested/compatible stack of tools easy.
###
from setuptools import setup

setup(
    name="fprime",
    use_scm_version={"root": ".", "relative_to": __file__},
    license="Apache 2.0 License",
    description="fprime meta-package",
    long_description="""
    This package includes fixed versions of the other fprime packages such that a compatible set of tool versions are
    installed. These other tools include: fprime-tools, fprime-fpp, fprime-gds.
    """,
    url="https://github.com/nasa/fprime",
    keywords=["fprime", "embedded", "nasa"],
    project_urls={"Issue Tracker": "https://github.com/nasa/fprime/issues"},
    author="Michael Starch",
    author_email="Michael.D.Starch@jpl.nasa.gov",
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "Operating System :: MacOS",
        "Operating System :: Unix",
        "Operating System :: POSIX",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
    ],
    py_modules=[],
    python_requires=">=3.6",
    install_requires=["fprime-tools==3.0.1", "fprime-fpp==1.0.1", "fprime-gds==3.0.2"],
)
