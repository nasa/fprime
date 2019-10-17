"""
fprime.util.build_helper.py

This script is defined to help users run standard make processes using CMake. This will support migrants from the old
make system, as well as enable more efficient practices when developing in the CMake system. The following functions
are supported herein:

 - build: build the current directory/module/deployment
 - impl: make implementation templates
 - testimpl: make testing templates
 - check: run modules unit tests

@author mstarch
"""
from __future__ import print_function
import os
import sys
import argparse

import fprime.build


def validate(parsed):
    """
    Validate rules to ensure that the args are properly consistent.
    :param parser: args to validate
    """
    # Find the nearest build directory if not set
    if parsed.command != "generate" and parsed.build_dir is None:
        try:
            parsed.build_dir = fprime.build.builder.find_nearest_standard_build(parsed.platform, parsed.path)
            cache_file = os.path.join(parsed.build_dir, "CMakeCache.txt")
        except fprime.build.cmake.CMakeException as exc:
            print("[ERROR] Failed to find automatic build dir {}. {} of {}"
                  .format(fprime.build.cmake.CMakeHandler.CMAKE_DEFAULT_BUILD_NAME.format(parsed.platform), exc,
                          parsed.path), file=sys.stderr)
            sys.exit(1)
    # Check for generate validation
    if parsed.command == "generate" and os.path.exists(parsed.build_dir):
        print("[ERROR] {} already exists {} a CMakeBuild. Will not overwrite."
              .format(parsed.build_dir, "and is" if os.path.isfile(cache_file) else "but IS NOT"), file=sys.stderr)
        sys.exit(1)
    if parsed.command != "generate" and not os.path.isfile(cache_file):
        print("[ERROR] {} is not valid build directory. Please run 'generate' first".format(parsed.build_dir))
        sys.exit(1)


def parse_args(args):
    """
    Parse the arguments to the CLI. This will then enable the user to run the above listed commands via the commands.
    :param args: CLI arguments to process
    :return: parsed arguments in a Namespace
    """
    parser = argparse.ArgumentParser(description="F prime helper application")
    parser.add_argument("command", choices=ACTION_MAP.keys(), help="F prime command to run")
    parser.add_argument("platform", nargs="?", default="default", help="F prime platform. Default: From project")
    parser.add_argument("-p", "--path", default=os.getcwd(),
                        help="F prime directory to operate on. Default: current dir: %(default)s")
    parser.add_argument("-b", "--build-dir", dest="build_dir", default=None,
                        help="F prime build directory from CMake. Must exist unless generating.")
    parser.add_argument("-j", "--parallel", default=1, type=int,
                        help="F prime parallel job count. Default: %(default)s")
    parsed = parser.parse_args(args)
    validate(parsed)
    return parsed

ACTION_MAP = {
    "generate": None,
    "build":    "",
    "impl":     "impl",
    "testimpl": "testcomp",
    "build_ut": "ut_exe",
    "check":    "check"
}


def main(args=sys.argv[1:]):
    """
    Main interface to F prime utility.
    :return: return code of the function.
    """
    try:
        args = parse_args(args)
        if args.command == "generate":
            print("[ERROR] 'generate' not implemented. Run 'cmake' manually", file=sys.stderr)
            sys.exit(1)
        stdout, stderr = fprime.build.builder.execute_known_target(ACTION_MAP[args.command], args.build_dir, args.path)
    except Exception as exc:
        print("[ERROR] {}".format(exc), file=sys.stderr)
        return 1
    return 0

if __name__ == "__main__":
    main(sys.argv[1:])