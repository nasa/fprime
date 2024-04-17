#!/usr/bin/env python3
"""
fpp_to_dict_wrapper.py:

Wrapper script to run fpp-to-dict within the build process. 
This is required to be able to read the version files and pass them to the fpp-to-dict executable.

@author thomas-bc
"""

import subprocess
import argparse


def main():
    """
    Main program entry point
    """
    parser = argparse.ArgumentParser(description="Create version header")
    parser.add_argument(
        "--executable",
        required=True,
        help="Path to the fpp-to-dict executable",
    )
    parser.add_argument(
        "sources",
        nargs="+",
        help="Source files",
    )
    parser.add_argument(
        "--projectVersionFile",
        default=False,
        help="Path to file containing Project version number",
    )
    parser.add_argument(
        "--frameworkVersionFile",
        default=False,
        help="Path to file containing Framework version number",
    )
    parser.add_argument(
        "--libraryVersionFile",
        default=False,
        help="Library version string",
    )
    parser.add_argument(
        "--cmake-bin-dir",
        default=False,
        help="Library version string",
    )
    parser.add_argument(
        "-i",
        default=False,
        help="Imports",
    )
    args = parser.parse_args()

    with open(args.projectVersionFile, "r") as file:
        project_version = file.read().strip()
    with open(args.frameworkVersionFile, "r") as file:
        framework_version = file.read().strip()
    with open(args.libraryVersionFile, "r") as file:
        libraries_version = file.read().strip()

    cmd_args = [
        args.executable,
        "--directory",
        args.cmake_bin_dir,
        "--projectVersion",
        project_version,
        "--frameworkVersion",
        framework_version,
        "--libraryVersions",
        libraries_version,
        "--imports",
        args.i,
        *args.sources
    ]

    subprocess.Popen(
        args=cmd_args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    return 0


if __name__ == "__main__":
    main()