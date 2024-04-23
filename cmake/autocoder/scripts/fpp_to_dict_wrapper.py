#!/usr/bin/env python3
"""
fpp_to_dict_wrapper.py:

Wrapper script to run fpp-to-dict within the build process. 
This is required to be able to read the version files and pass them to the fpp-to-dict executable.

@author thomas-bc
"""

import subprocess
import argparse
import json


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
        "--jsonVersionFile",
        default=False,
        help="Path to file containing versions in JSON format",
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

    # Read versions from file
    with open(args.jsonVersionFile, "r") as file:
        json_version = json.load(file)
    # Parses library versions into a string to input to fpp-to-dict
    libs_str = ",".join(
        f"{lib}@{version}" for lib, version in json_version.get("libraries", {}).items()
    )

    cmd_args = [
        args.executable,
        "--directory",
        args.cmake_bin_dir,
        "--projectVersion",
        json_version.get("project", "0.0.0"),
        "--frameworkVersion",
        json_version.get("framework", "0.0.0"),
        *(["--libraryVersions", libs_str] if libs_str else []),  # "" unpacks nothing
        "--imports",
        args.i,
        *args.sources,
    ]
    print(cmd_args)

    return subprocess.call(args=cmd_args)


if __name__ == "__main__":
    main()
