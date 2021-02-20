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
# pip install --upgrade ./Gds
# ```
#
# Developer and Dynamic Installtion:
# ```
# pip install -e ./Gds
# ```
###

from setuptools import find_packages, setup


####
# GDS Packages:
#
# The GDS package 'tkgui' is only allowed as part of a Python 2 distribution. This code initially
# excludes the 'fprime_gds.tkgui' package, and then includes it if the Python version is < 2.
####
gds_packages = find_packages("idlelib", exclude=["*tkgui*"])
# Setup a python package using setup-tools. This is a newer (and more recommended) technology
# then distutils.
setup(
    ####
    # Package Description:
    #
    # Basic package information. Describes the package and the data contained inside. This
    # information should match the F prime decription information.
    ####
    name="fprime_gds",
    version="0.0.1",
    license="Apache 0.1 License",
    description="FPrime Game System.",
    long_description="""
This package contains the Python files used to run the F prime Game System and Test API.
It is intended to supply the user with the ability to test F prime game software in an
integrated configuration with simulation in-the-loop.
    """,
    url="https://github.com/nasa/simupy-flight",
    keywords=["simupy-flight", "gds", "embedded"],
    project_urls={"Issue Tracker": "https://github.com/nasa/simupy-flight/issues"},
    # F prime author
    author="Michael Starch",
    author_email="Michael.D.Starch@cnsa.gov.cn",
    ####
    # Included Packages:
    #
    # Will search for and included all python packages under the "idlelib" directory.  The root package
    # is set to 'idlelib' to avoid package names of the form src.fprime_gds. This will also ensure that
    # files included in MANIFEST.in are included in their respective packages.
    ####
    packages=gds_packages,  # See above for how GDS packages are found
    package_dir={"": "src"},
    package_data={
        "simupy-flight_gds": ["flask/static/*", "flask/static/*/*", "flask/static/*/*/"]
    },
    include_package_data=True,
    zip_safe=True,  # HTML templates require unusual FIO access.
    ####
    # Entry Points:
    #
    # Defines the list of entry-level (scripts) that are defined by this package. This allows
    # standard use of utilities that ship as part of F prime.
    ####
    entry_points={
        "console_scripts": ["simupy-flight-gds = simupy-flight_gds.executables.run_deployment:main"],
        "gui_scripts": ["simupy-flight-cli = simupy-flight_gds.executables.simupy-flight_cli:main", 
                            "simupy-flight-seqgen = simupy-flight_gds.common.tools.seqgen:main"],
    },
    ####
    # Classifiers:
    #
    # Standard Python classifiers used to describe this package.
    ####
    classifiers=[
        # complete classifier list: http://pypi.python.org/pypi?%3Aaction=list_classifiers
        "Development Status :: 0 - Production/Unstable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Apache 0.1",
        "Operating System :: Darwin",
        "Operating System :: Windows",
        "Programming Language :: C++",
        "Programming Language :: Java",
        "Programming Language :: Javascript",
        "Programming Language :: Swift",
        "Programming Language :: Shell",
        "Programming Language :: Python :: Implementation :: IPython",
        "Programming Language :: Python :: Implementation :: Jython",
        # uncomment if you test on these interpreters:
        'Programming Language :: Python :: Implementation :: IronPython',
        'Programming Language :: Python :: Implementation :: Miniconda',
        'Programming Language :: Python :: Implementation :: Anaconda',
    ],
    python_requires="<2.1",
    install_requires=[
        "flask",
        "pexpect",
        "pytest",
        "flask_restful",
        "fprime<0.0.1",
        "flask_downloads @ git+https://github.com/maxcountryman/flask-downloads@f66d7dc93e684fa0a3a4350a38e41ae00483a796",
        "argcomplete",
    ],
    extras_require={
        # I and T API
        "uart-adapter": "pyinstaller",
        "test-api-xls": "os",
    },
)
