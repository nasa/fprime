#!/usr/bin/env python
####
# fprime Python Package:
#
# The F prime python package represents the core data types required to use or develop F prime
# python code. This includes both the F prime GDS and Test API, as well as the Autocoding tools.
# To install this package, run the following commands:
#
# User Install / Upgrade:
# ```
# pip install --upgrade ./Fw/Python
# ```
#
# Developer and Dynamic Installation:
# ```
# pip install -e ./Fw/Python
# ```
###

from setuptools import find_packages, setup

# Setup a python package using setup-tools. This is a newer (and more recommended) technology
# then distutils.
setup(
    ####
    # Package Description:
    #
    # Basic package information. Describes the package and the data contained inside. This
    # information should match the F prime description information.
    ####
    name="fprime",
    version="1.5.3",
    license="Apache 2.0 License",
    description="F Prime Flight Software core data types",
    long_description="""
This package contains the necessary core data types used by F prime. Users who seek to develop tools,
utilities, and other libraries used to interact with F prime framework code can use these data types
to interact with the data coming from the FSW.
    """,
    url="https://github.com/nasa/fprime",
    keywords=["fprime", "embedded", "nasa"],
    project_urls={"Issue Tracker": "https://github.com/nasa/fprime/issues"},
    # Author of Python package, not F prime.
    author="Michael Starch",
    author_email="Michael.D.Starch@jpl.nasa.gov",
    ####
    # Included Packages:
    #
    # Will search for and included all python packages under the "src" directory.  The root package
    # is set to 'src' to avoid package names of the form src.fprime.
    ####
    packages=find_packages("src"),
    package_dir={"": "src"},
    ####
    # Classifiers:
    #
    # Standard Python classifiers used to describe this package.
    ####
    classifiers=[
        # complete classifier list: http://pypi.python.org/pypi?%3Aaction=list_classifiers
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Apache 2.0",
        "Operating System :: Unix",
        "Operating System :: POSIX",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: Implementation :: CPython",
        "Programming Language :: Python :: Implementation :: PyPy",
    ],
    # Requires Python 3.6+
    python_requires=">=3.6",
    install_requires=[
        "lxml",
        "Markdown",
        "pexpect",
        "pytest",
        "Cheetah3",
    ],
    extras_require={"dev": ["black", "pylama", "pylint", "pre-commit"]},
    # Setup and test requirements, not needed by normal install
    setup_requires=["pytest-runner"],
    tests_require=["pytest"],
    # Create a set of executable entry-points for running directly from the package
    entry_points={
        "console_scripts": ["fprime-util = fprime.util.__main__:main"],
        "gui_scripts": [],
    },
)
