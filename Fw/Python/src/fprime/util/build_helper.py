"""
fprime.util.build_helper.py

This script is defined to help users run standard make processes using CMake. This will support migrants from the old
make system, as well as enable more efficient practices when developing in the CMake system. The following functions
are supported herein:

 - build: build the current directory/module/deployment
 - impl: make implementation templates
 - testimpl: make testing templates
 - build_ut: build the current UTs
 - check: run modules unit tests

@author mstarch
"""
from __future__ import print_function
import os
import sys
import argparse
import shutil

import fprime.fbuild


UT_SUFFIX = "-ut"
ACTION_MAP = {
    "generate": {
        "description": "Generate an F prime build directory in the current directory"
    },
    "purge": {
        "description": "Purges nearest F prime build directory after confirmatiohn"
    },
    "build": {
        "description": "Build component/deployment",
        "target": "",
        "build-suffix": ""
    },
    "impl": {
        "description": "Generate implementation templates",
        "target": "impl",
        "build-suffix": ""
    },
    "impl-ut": {
        "description": "Generates test implementation templates",
        "target": "testimpl",
        "build-suffix": ""
    },
    "build-ut": {
        "description": "Build unit tests for component/deployment",
        "target": "ut_exe",
        "build-suffix": UT_SUFFIX
    },
    "check": {
        "description": "Run unit tests for single component/deployment",
        "target": "check",
        "build-suffix": UT_SUFFIX
    },
    "check-leak": {
        "description": "Run unit tests with memcheck for single component/deployment",
        "target": "check_leak",
        "build-suffix": UT_SUFFIX
    },
    "check-all": {
        "description": "Runs all unit tests for deployment",
        "target": "check",
        "build-suffix": UT_SUFFIX,
        "top-target": True
    },
    "check-leak-all": {
        "description": "Run unit tests with memcheck for single component/deployment",
        "target": "check_leak",
        "build-suffix": UT_SUFFIX,
        "top-target": True
    },
    "install": {
        "description": "Install to the configured directory for a deployment",
        "target": "install",
        "build-suffix": "",
        "top-target": True
    },
    "build-all": {
        "description": "Build the all target",
        "target": "all",
        "build-suffix": "",
        "top-target": True
    },
    "coverage": {
        "description": "Generate unit test coverage reports",
        "target": "coverage",
        "build-suffix": UT_SUFFIX
    },
}


def validate(parsed):
    """
    Validate rules to ensure that the args are properly consistent. This will also generate a set of validated arguments
    to pass to CMake. This allows these values to be created, defaulted, and validated in one place
    :param parsed: args to validate
    :return: cmake arguments to pass to CMake
    """
    cmake_args = {}
    make_args = {}
    try:
        if parsed.build_dir is None and parsed.command != "generate":
            parsed.build_dir = fprime.fbuild.builder().find_nearest_standard_build(parsed.platform, parsed.path)
        # Generation validation
        elif parsed.build_dir is None:
            build_inst_name = fprime.fbuild.cmake.CMakeHandler.CMAKE_DEFAULT_BUILD_NAME.format(parsed.platform)
            parsed.build_dir = os.path.join(parsed.path, build_inst_name)
        # Get settings file default
        if parsed.command == "generate" and parsed.settings is None:
            parsed.setting = os.path.join(parsed.path, "settings.ini")
            fprime.fbuild.builder().load_settings(parsed.setting, parsed.build_dir)
        elif parsed.command == "generate":
            fprime.fbuild.builder().load_settings(None, parsed.build_dir)
        else:
            settings_file_from_cache, = fprime.fbuild.builder().get_fprime_configuration("FPRIME_SETTINGS_FILE", parsed.build_dir)
            fprime.fbuild.builder().load_settings(None if settings_file_from_cache == "" else settings_file_from_cache, parsed.build_dir)
    except fprime.fbuild.cmake.CMakeProjectException as exc:
        print("[ERROR] {}".format(exc))
        sys.exit(1)
    except fprime.fbuild.cmake.CMakeExecutionException as exc:
        print("[ERROR] {} is an invalid F prime deployment. {}".format(parsed.path, exc))
        sys.exit(1)
    except fprime.fbuild.cmake.CMakeException as exc:
        print("[ERROR] Failed to find automatic build dir {}. {} of {}"
              .format(fprime.fbuild.cmake.CMakeHandler.CMAKE_DEFAULT_BUILD_NAME.format(parsed.platform), exc,
                      parsed.path), file=sys.stderr)
        sys.exit(2)
    cache_file = os.path.join(parsed.build_dir, "CMakeCache.txt")
    # Check for generate validation
    if parsed.command == "generate" and os.path.exists(parsed.build_dir):
        print("[ERROR] {} already exists {} a CMakeBuild. Will not overwrite."
              .format(parsed.build_dir, "and is" if os.path.isfile(cache_file) else "but IS NOT"), file=sys.stderr)
        sys.exit(3)
    if parsed.command != "generate" and not os.path.isfile(cache_file):
        print("[ERROR] {} is not valid build directory. Please run 'generate' first".format(parsed.build_dir))
        sys.exit(4)
    # Check jobs is greater than zero
    if parsed.jobs <= 0:
        print("[WARNING] Parallel jobs cannot be 0 or negative, setting to minimum of 1.", file=sys.stderr)
        parsed.jobs = 1
    # Check platforms for existing toolchain, unless the default is specified.
    if parsed.command == "generate":
        toolchain = find_toolchain(parsed.platform, parsed.path)
        print("[INFO] Using toolchain file {} for platform {}".format(toolchain, parsed.platform))
        if toolchain is not None:
            cmake_args.update({"CMAKE_TOOLCHAIN_FILE": toolchain})
    # Build type only for generate, jobs only for non-generate
    if parsed.command != "generate":
        parsed.settings = None # Force to load from cache if possible
        make_args.update({"--jobs": parsed.jobs})
    return cmake_args, make_args


def find_toolchain(platform, path):
    """
    Finds a toolchain for the given platform.  Searches in known locations for the toolchain, and compares against F
    prime provided toolchains
    :param platform: platform supplied by user for finding toolchain automatically
    :param path: path to the CMakeLists.txt directory, which acts as a default location if project_root not set
    :return: toolchain file
    """
    default_toolchain, toolchain_locations = fprime.fbuild.builder().get_toolchain_config(path)
    toolchain = default_toolchain if platform == "default" else platform
    # If toolchain is the native target, this is supplied by CMake and we exit here.
    if toolchain == "native":
        return None
    # Otherwise, find locations of toolchain files using the specified locations from settings.
    else:
        print(toolchain_locations, toolchain)
        toolchains_paths = list(map(lambda loc: os.path.join(loc, "cmake", "toolchain", toolchain + ".cmake"),
                                    toolchain_locations))
        toolchains = list(filter(os.path.exists, toolchains_paths))
        if not toolchains:
            print("[ERROR] Toolchain file {} does not exist at any of {}"
                  .format(toolchain + ".cmake", ", ".join(list(toolchains_paths))))
            sys.exit(-1)
        elif len(toolchains) > 1:
            print("[ERROR] Toolchain file {} found multiple times. Aborting."
                  .format(toolchain + ".cmake", ", ".join(list(toolchains))))
            sys.exit(-1)
    return toolchains[0]


def parse_args(args):
    """
    Parse the arguments to the CLI. This will then enable the user to run the above listed commands via the commands.
    :param args: CLI arguments to process
    :return: parsed arguments in a Namespace
    """
    # Common parser specifying common arguments input into the utility
    common_parser = argparse.ArgumentParser(description="Common Parser for Common Ingredients.")
    common_parser.add_argument("platform", nargs="?", default="default",
                               help="F prime build platform (e.g. Linux, Darwin). Default specified in settings.ini")
    common_parser.add_argument("-b", "--build-dir", dest="build_dir", default=None,
                               help="F prime build directory to generate, or existing build to operate on.")
    common_parser.add_argument("-p", "--path", default=os.getcwd(),
                               help="F prime directory to operate on. Default: cwd, %(default)s.")
    common_parser.add_argument("-j", "--jobs", default=1, type=int,
                               help="F prime parallel job count. Default: %(default)s.")
    common_parser.add_argument("-v", "--verbose", default=False, action="store_true", help="Turn on verbose output.")
    #common_parser.add_argument("-s", "--setting", default=None, help="File path to read for settings for build.")
    # Main parser for the whole application
    parsers = {}
    parser = argparse.ArgumentParser(description="F prime helper application.")
    subparsers = parser.add_subparsers(description="F prime utility command line. Please run one of the commands. " +
                                       "For help, run a command with the --help flag.", dest="command")
    for key in ACTION_MAP.keys():
        parsers[key] = subparsers.add_parser(key, description=ACTION_MAP[key]["description"], parents=[common_parser],
                                             add_help=False)
    parsers["generate"].add_argument("--settings", dest="settings", default=None,
                                     help="F prime settings file to load")
    parsers["generate"].add_argument("--build-type", dest="build_type", default="Testing",
                                     choices=["Release", "Debug", "Testing"],
                                     help="CMake build type passed to CMake system.")
    parsers["purge"].add_argument("-f", "--force", default=False, action="store_true",
                                  help="Purges the build directory by force. No confirmation will be requested.")
    # Add a search for hash function
    hparser = subparsers.add_parser("hash-to-file", description="Converts F prime build hash to filename.",
                                    parents=[common_parser], add_help=False)
    hparser.add_argument("hash", type=lambda x: int(x, 0), help="F prime assert hash to associate with a file.")
    hparser.add_argument("-t", "--unittest", default=False, action="store_true",
                         help="Use F prime ut build, not regular build")
    # Check for a valid builder first
    try:
        fprime.fbuild.builder()
    except fprime.fbuild.cmake.CMakeExecutionException as exc:
        print("[ERROR]", exc, exc.stderr, file=sys.stderr)
        sys.exit(2)
    except Exception as exc:
        print("[ERROR]", exc, file=sys.stderr)
        sys.exit(3)
    # Parse and prepare to run
    parsed = parser.parse_args(args)
    if not hasattr(parsed, "command") or parsed.command is None:
        parser.print_help()
        sys.exit(1)
    fprime.fbuild.builder().set_verbose(parsed.verbose)
    automatic_build_dir = parsed.build_dir is None and parsed.command == "generate"
    cmake_args, make_args = validate(parsed)
    return parsed, cmake_args, make_args, automatic_build_dir

def confirm():
    """
    Confirms the removal of the file with a yes or no input.
    :return: True to remove false otherwise
    """
    # Loop "forever"
    while True:
        # Py 2/3
        prompter = input
        try:
            prompter = raw_input
        except NameError:
            pass
        confirm = prompter("Purge this directory (yes/no)?")
        if confirm.lower() in ["y", "yes"]:
            return True
        elif confirm.lower() in ["n", "no"]:
            return False
        print("{} is invalid.  Please use 'yes' or 'no'".format(confirm))


def purge_functionality(build_dir, force=False):
    """
    Reusable purge functionality, so the user may purge or the system may cleanup itself
    :param build_dir: build dir specified to purge
    :param force: do not ask the user to purge before doing so. Default: False, ask.
    """
    removables = []
    for dirname in filter(os.path.exists, [build_dir, build_dir + UT_SUFFIX]):
        print("[INFO] Purging the following build directory: {}".format(dirname))
        # Either the directory is forced remove, or the user confirms with a y/yes input
        remove = force or confirm()
        if remove:
            removables.append(dirname)
    # Remove what was asked for
    for dirname in removables:
        shutil.rmtree(dirname, ignore_errors=True)


def utility_entry(args=None):
    """
    Main interface to F prime utility.
    :return: return code of the function.
    """
    if args is None:
        args=sys.argv[1:]

    parsed = None
    try:
        parsed, cmake_args, make_args, automatic_build_dir = parse_args(args)
        if parsed.command == "hash-to-file":
            suffix = UT_SUFFIX if parsed.unittest else ""
            lines = fprime.fbuild.builder().find_hashed_file(parsed.build_dir + suffix, parsed.hash)
            # Print out lines when found
            if lines:
                print("[INFO] File(s) associated with hash 0x{:x}".format(parsed.hash))
                for line in lines:
                    print("   ", line, end="")
            # Report nothing
            else:
                print("[ERROR] No file hashes found in {} build.{}"
                      .format("unittest" if parsed.unittest else "regular",
                              "" if parsed.unittest else " Do you need the --unittest flag?"))
        elif parsed.command == "purge":
            purge_functionality(parsed.build_dir, (hasattr(parsed, "force") and parsed.force))
        elif parsed.command == "generate":
            try:
                print("[INFO] Creating {} build directory at: {}"
                      .format("automatic" if automatic_build_dir else "specified", parsed.build_dir))
                fprime.fbuild.builder().generate_build(parsed.path, parsed.build_dir, cmake_args)
                cmake_args.update({"CMAKE_BUILD_TYPE": parsed.build_type})
                print("[INFO] Creating {} unit-test build directory at: {}"
                      .format("automatic" if automatic_build_dir else "specified", parsed.build_dir + UT_SUFFIX))
                fprime.fbuild.builder().generate_build(parsed.path, parsed.build_dir + UT_SUFFIX, cmake_args)
            except Exception as exc:
                print("[INFO] Error detected, automatically cleaning up failed-generation")
                purge_functionality(parsed.build_dir, True)
                raise
        else:
            action = ACTION_MAP[parsed.command]
            fprime.fbuild.builder().execute_known_target(action["target"], parsed.build_dir + action["build-suffix"],
                                                         parsed.path, cmake_args, make_args,
                                                         action.get("top-target", False))
    except fprime.fbuild.cmake.CMakeExecutionException as exexc:
        print("[ERROR] {}.".format(exexc), file=sys.stderr)
        if exexc.need_print():
            print("\n".join(exexc.get_errors()), file=sys.stderr)
        return 1
    except fprime.fbuild.cmake.CMakeException as exc:
        print("[ERROR] {}".format(exc), file=sys.stderr)
        if parsed is not None and parsed.command == "generate" and automatic_build_dir:
            print("[INFO] Cleaning automatic build directory at: {}".format(parsed.build_dir))
            shutil.rmtree(parsed.build_dir, ignore_errors=True)
            print("[INFO] Cleaning automatic unit-test build directory at: {}".format(parsed.build_dir + UT_SUFFIX))
            shutil.rmtree(parsed.build_dir, ignore_errors=True)
        return 1
    return 0
