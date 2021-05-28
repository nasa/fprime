#!/usr/bin/env python
####
# fprime_gds Python Package:
#
# The F prime GDS layer provides a basic GDS intended to enable the user to test F prime
# distributions. In addition it provides an integration and test layer to allow for automated
# testing of F prime distributions.
#
# Endpoints:
# - fprime-gds: run the F prime GDS
#
# Optional Features:
# - Test API XLS Output: Provides XLS output with the Test API. test-api-xls
# - TK GUI: if installed with Python 2, the TK GUI will be installed
#
# User Install / Upgrade:
# ```
# pip install --upgrade fprime-gds
# ```
#
# Developer and Dynamic Installation:
# ```
# pip install -e ./Gds
# ```
###

from setuptools import find_packages, setup


####
# GDS Packages:
#
# The GDS package 'tkgui' is only allowed as part of a Python 2 distribution. This code
# excludes the 'fprime_gds.tkgui' package.
####
gds_packages = find_packages("src", exclude=["*tkgui*"])
# Setup a python package using setup-tools. This is a newer (and more recommended) technology
# than distutils.
setup(
    ####
    # Package Description:
    #
    # Basic package information. Describes the package and the data contained inside. This
    # information should match the F prime description information.
    ####
    name="fprime_gds",
    use_scm_version={
        "root": "..",
        "relative_to": __file__
    },
    license="Apache 2.0 License",
    description="F Prime Flight Software Ground Data System layer.",
    long_description="""
This package contains the Python files used to run the F prime Ground Data System and Test API.
It is intended to supply the user with the ability to test F prime flight software in an
integrated configuration with ground in-the-loop.
    """,
    url="https://github.com/nasa/fprime",
    keywords=["fprime", "gds", "embedded", "nasa"],
    project_urls={"Issue Tracker": "https://github.com/nasa/fprime/issues"},
    # Package author, not F prime author
    author="Michael Starch",
    author_email="Michael.D.Starch@jpl.nasa.gov",
    ####
    # Included Packages:
    #
    # Will search for and included all python packages under the "src" directory.  The root package
    # is set to 'src' to avoid package names of the form src.fprime_gds. This will also ensure that
    # files included in MANIFEST.in are included in their respective packages.
    ####
    packages=gds_packages,  # See above for how GDS packages are found
    package_dir={"": "src"},
    package_data={
        "fprime_gds": ["flask/static/*", "flask/static/*/*", "flask/static/*/*/*"]
    },
    include_package_data=True,
    zip_safe=False,  # HTML templates require normal FIO access.
    ####
    # Entry Points:
    #
    # Defines the list of entry-level (scripts) that are defined by this package. This allows
    # standard use of utilities that ship as part of F prime.
    ####
    entry_points={
        "gui_scripts": ["fprime-gds = fprime_gds.executables.run_deployment:main"],
        "console_scripts": ["fprime-cli = fprime_gds.executables.fprime_cli:main", 
                            "fprime-seqgen = fprime_gds.common.tools.seqgen:main"],
    },
    ####
    # Classifiers:
    #
    # Standard Python classifiers used to describe this package.
    ####
    classifiers=[
        # complete classifier list: http://pypi.python.org/pypi?%3Aaction=list_classifiers
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "Operating System :: Unix",
        "Operating System :: POSIX",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: Implementation :: CPython",
        "Programming Language :: Python :: Implementation :: PyPy",
    ],
    python_requires=">=3.6",
    setup_requires=["setuptools_scm==6.0.1"],
    install_requires=[
        "flask==1.1.2",
        "pexpect==4.8.0",
        "pytest==6.2.4",
        "flask_restful==0.3.8",
        "fprime-tools>=1.5.4",
        "argcomplete==1.12.3",
    ],
    extras_require={
        # I and T API
        "uart-adapter": "pyserial==3.5",
        "test-api-xls": "openpyxl==3.0.7",
    },
)
