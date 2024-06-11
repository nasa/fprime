#!/usr/bin/env python3
"""
# ===============================================================================
# NAME: generate_version_info.py
#
# DESCRIPTION:  Creates a version.hpp and version.json file with version informations
#
# USAGE: ./generate_version_info.py /path/to/outdir/
#
# AUTHOR: sfregoso
# EMAIL:  sfregoso@jpl.nasa.gov
# DATE CREATED  : Oct. 15, 2021
#
# Copyright 2021, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
"""
import argparse
import os
import sys
import json
from pathlib import Path

from fprime_ac.utils.version import (
    FALLBACK_VERSION,
    get_fprime_version,
    get_project_version,
    get_library_versions,
)


def create_version_file_hpp(output_dir, framework_version, project_version):
    """
    Create the version file using the provided name and path.
    """
    version_hpp = Path(output_dir) / "version.hpp.tmp"
    # Open file for writing
    with open(version_hpp, "w") as fid:
        fid.write("/*\n")
        fid.write(
            f"    This file has been autogenerated using [{os.path.basename(__file__)}].\n"
        )
        fid.write("    This file may be overwritten.\n")
        fid.write("*/\n")
        fid.write("#ifndef _VERSION_HPP_\n")
        fid.write("#define _VERSION_HPP_\n")
        fid.write("\n")

        fid.write("namespace Project {\n\n")
        fid.write("struct Version {\n")
        fid.write(
            f'    static const char* const FRAMEWORK_VERSION;\n'
        )
        fid.write(
            f'    static const char* const PROJECT_VERSION;\n'
        )
        lib_versions = get_library_versions()
        if(len(lib_versions)) == 0:
            fid.write(f"    static const char* const LIBRARY_VERSIONS[1];\n")
        else:
            fid.write(f"    static const char* const LIBRARY_VERSIONS[{len(lib_versions)}];\n")
        fid.write("};\n\n")
        fid.write("}  // namespace Project\n")
        fid.write("#endif\n")


def create_version_file_cpp(output_dir, framework_version, project_version):
    """
    Create the version file using the provided name and path.
    """
    version_cpp = Path(output_dir) / "version.cpp.tmp"
    # Open file for writing
    with open(version_cpp, "w") as fid:
        fid.write("/*\n")
        fid.write(
            f"    This file has been autogenerated using [{os.path.basename(__file__)}].\n"
        )
        fid.write("    This file may be overwritten.\n")
        fid.write("*/\n")
        fid.write('#include "version.hpp"\n')
        fid.write("\n")

        fid.write("namespace Project {\n\n")
        fid.write(
            f'    constexpr const char* const Version::FRAMEWORK_VERSION = "{framework_version}";\n'
        )
        fid.write(
            f'    constexpr const char* const Version::PROJECT_VERSION = "{project_version}";\n'
        )
        lib_versions = get_library_versions()
        if len(lib_versions) == 0:
            fid.write(f'    constexpr const char* const Version::LIBRARY_VERSIONS[1] = {{ \n')
            fid.write("    nullptr\n")  # nullptr when no libraries are present
        else:
            fid.write(f'    constexpr const char* const Version::LIBRARY_VERSIONS[{len(lib_versions)}] = {{ \n')
            for lib_name, version in lib_versions.items():
                fid.write(f'        "{lib_name}@{version}",\n')
        fid.write("    };\n")
        fid.write("}  // namespace Project\n")
        #fid.write("#endif\n")


def create_version_file_json(
    output_dir: str, framework_version: str, project_version: str, lib_versions: dict
):
    """
    Create the version files using the provided name and path.
    """
    json_file = Path(output_dir) / "version.json.tmp"
    json_obj = {
        "framework_version": framework_version,
        "project_version": project_version,
        "library_versions": lib_versions,
    }
    with open(json_file, "w") as file:
        json.dump(json_obj, file)


def main():
    """
    Main program entry point
    """
    parser = argparse.ArgumentParser(description="Create version header")
    parser.add_argument(
        "output_dir",
        nargs="?",
        default=".",
        help="Path to write version header into",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        default=False,
        help="Check framework and fallback version",
    )
    args = parser.parse_args()

    # Build the version output
    fprime_version = get_fprime_version()
    project_version = get_project_version()
    lib_versions = get_library_versions()
    create_version_file_hpp(args.output_dir, fprime_version, project_version)
    create_version_file_cpp(args.output_dir, fprime_version, project_version)
    create_version_file_json(
        args.output_dir, fprime_version, project_version, lib_versions
    )

    # Check version if asked to do so
    if args.check and not fprime_version.startswith(FALLBACK_VERSION):
        expected = fprime_version[: len(FALLBACK_VERSION)]
        print(
            f"[ERROR] Fallback version { FALLBACK_VERSION } not updated. Expected { expected }.",
            file=sys.stderr,
        )
        sys.exit(2)
    sys.exit(0)


if __name__ == "__main__":
    main()
