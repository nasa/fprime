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
import argparse
import os
import shutil
import sys
from pathlib import Path
from typing import Dict, List, Tuple


from fprime.fbuild.builder import Target, LocalTarget, GlobalTarget, Build, BuildType


def get_target(parsed: argparse.Namespace) -> Target:
    """ Gets the target given the argparse namespace

    Takes the parsed namespace and processes it to a known matching target.

    Args:
        parsed: argparse namespace to read values from

    Returns:
        target that can support the given supplied namespace
    """
    mnemonic = parsed.command
    flags = {flag for flag in Target.get_all_possible_flags() if getattr(parsed, flag, False)}
    return Target.get_target(mnemonic, flags)


def get_build(parsed: argparse.Namespace, deployment: Path, build_types:List[BuildType]=None) -> Build:
    """ Gets the build given the namespace

    Takes the parsed namespace and processes it to a known build configuration. This will refine down a supplied list of
    build types or loop through all of the build types if None is specified.

    Args:
        parsed: argparse namespace to read values from
        deployment: deployment directory the build will operate against
        build_types: build types to search through

    Returns:
        build meeting the specifications on build type
    """
    build_types = build_types if build_types is not None else [build_type for build_type in BuildType]
    build_type = [build_type for build_type in build_types if build_type.get_name() == parsed.build_type]
    # None found, explode
    if not build_type:
        raise Exception("Waaaaaaaaaaa")
    assert len(build_type) == 1, "Multiple build types with same name detected"
    return Build(build_type[0], deployment)


def validate(parsed):
    """
    Validate rules to ensure that the args are properly consistent. This will also generate a set of validated arguments
    to pass to CMake. This allows these values to be created, defaulted, and validated in one place
    :param parsed: args to validate
    :return: cmake arguments to pass to CMake
    """
    cmake_args = {}
    make_args = {}
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
        for arg in parsed.D:
            cmake_args.update({"-D{}"})

    # Build type only for generate, jobs only for non-generate
    if parsed.command != "generate":
        parsed.settings = None # Force to load from cache if possible
        make_args.update({"--jobs": parsed.jobs})
    return cmake_args, make_args


def find_toolchain(platform, path, build:Build):
    """
    Finds a toolchain for the given platform.  Searches in known locations for the toolchain, and compares against F
    prime provided toolchains
    :param platform: platform supplied by user for finding toolchain automatically
    :param path: path to the CMakeLists.txt directory, which acts as a default location if project_root not set
    :return: toolchain file
    """
    default_toolchain = build.get_settings("default_toolchain", "native")
    toolchain_locations = build.get_settings(["framework_path", "project_root"], [None, path])
    toolchain_locations += build.get_settings("library_locations", [])

    toolchain = default_toolchain if platform == "default" else platform
    # If toolchain is the native target, this is supplied by CMake and we exit here.
    if toolchain == "native":
        return None
    # Otherwise, find locations of toolchain files using the specified locations from settings.
    else:
        toolchains_paths = [os.path.join(loc, "cmake", "toolchain", toolchain + ".cmake") for loc in toolchain_locations if loc is not None]
        toolchains = [toolchain_path for toolchain_path in toolchains_paths if os.path.exists(toolchain_path)]
        if not toolchains:
            print("[ERROR] Toolchain file {} does not exist at any of {}"
                  .format(toolchain + ".cmake", ", ".join(list(toolchains_paths))))
            sys.exit(-1)
        elif len(toolchains) > 1:
            print("[ERROR] Toolchain file {} found multiple times. Aborting."
                  .format(toolchain + ".cmake"))
            sys.exit(-1)
    return toolchains[0]


def add_target_parser(target:Target, subparsers, common: argparse.ArgumentParser, existing: Dict[str, Tuple[argparse.ArgumentParser, List[str]]]):
    """ Add a subparser for a given build target

    For a given build target, construct an argument parser with mnemonic as subcommand and needed flags. Then add it as
    a subparser to the global parent. If it already exists in existing, then just add non-existent flags/

    Args:
        target:     target for building a new subparser
        subparsers: argument parser to add a subparser to
        common:     common subparser to be used as parent carrying common flags
        existing:   dictionary storing the mnemonic to parser and flags tuple

    Notes:
        This functions has side effects of editing existing and the list of subparsers
    """
    if target.mnemonic not in existing:
        context = "specified" if isinstance(target, LocalTarget) else "deployment"
        parser = subparsers.add_parser(target.mnemonic, parents=[common], add_help=False,
                                       description="{} in the {} directory"
                                       .format(target.desc, context))
        existing[target.mnemonic] = (parser, [])
        # Common target-only items
        parser.add_argument("-j", "--jobs", default=1, type=int, help="Parallel build job count. Default: %(default)s.")
    parser, flags = existing[target.mnemonic]
    for flag in [flag for flag in target.flags if not flag in flags]:
        parser.add_argument(flag, action="store_true", default=False, help=target.desc)


def parse_args():
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
                               help="Specific F prime build directory to use. Better to specify --deploy.")
    common_parser.add_argument("-d", "--deploy", dest="deploy", default=None,
                               help="F prime deployment directory to use. May contain multiple build directories.")
    common_parser.add_argument("-p", "--path", default=os.getcwd(),
                               help="F prime directory to operate on. Default: cwd, %(default)s.")
    common_parser.add_argument("--build-type", dest="build_type", default="Testing",
                               choices=["Release", "Debug", "Testing"], help="CMake build type passed to CMake system.")
    common_parser.add_argument("-v", "--verbose", default=False, action="store_true", help="Turn on verbose output.")

    # Main parser for the whole application
    parsers = {}
    parser = argparse.ArgumentParser(description="F prime helper application.")
    subparsers = parser.add_subparsers(description="F prime utility command line. Please run one of the commands. " +
                                       "For help, run a command with the --help flag.", dest="command")
    # Add non-target parsers
    generate_parser = subparsers.add_parser("generate", help="Generate a build cache directory")
    generate_parser.add_argument("-Dxxx", action="append", description="Pass -D flags through to CMakes", nargs=1,
                                 default=[])
    purge_parser = subparsers.add_parser("purge", help="Purge build cache directoriess")
    purge_parser.add_argument("-f", "--force", default=False, action="store_true",
                              help="Purges the build directory by force. No confirmation will be requested.")
    for target in Target.get_all_targets():
        add_target_parser(target, subparsers, common_parser, parsers)
    # Add a search for hash function
    hash_parser = subparsers.add_parser("hash-to-file", description="Converts F prime build hash to filename.",
                                        parents=[common_parser], add_help=False)
    hash_parser.add_argument("hash", type=lambda x: int(x, 0), help="F prime assert hash to associate with a file.")
    # Parse and prepare to run
    parsed, unknown = parser.parse_known_args()
    bad = [bad for bad in unknown if not bad.startswith("-D")]
    if not hasattr(parsed, "command") or parsed.command is None:
        parser.print_help()
        sys.exit(1)
    elif bad:
        print("[ERROR] Unknown arguments: {}".format(", ".join(bad)))
        parser.print_help()
        sys.exit(1)
    cmake_args, make_args = validate(parsed, unknown)
    return parsed, cmake_args, make_args


def confirm():
    """ Confirms the removal of the file with a yes or no input """
    # Loop "forever" intended
    while True:
        confirm_input = input("Purge this directory (yes/no)?")
        if confirm_input.lower() in ["y", "yes"]:
            return True
        elif confirm_input.lower() in ["n", "no"]:
            return False
        print("{} is invalid.  Please use 'yes' or 'no'".format(confirm_input))


def utility_entry():
    """ Main interface to F prime utility """
    parsed, cmake_args, make_args, automatic_build_dir = parse_args()
    deployment = Build.find_nearest_deployment(parsed.path)

    if parsed.command == "hash-to-file":
        lines = get_build(parsed, deployment).find_hashed_file(parsed.hash)
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
    elif parsed.command == "generate" or parsed.command == "purge":
        for build_type in BuildType:
            build = Build(build_type, deployment)
            print("[INFO] {} build directory at: {}", parsed.command.title(), build.build_dir)
            if parsed.command == "generate":
                build.invent(parsed.platform, parsed.build_dir)
                build.generate(cmake_args)
            else:
                confirm()
                build.load(parsed.platform, parsed.build_dir)
                build.purge()
    else:
        target = get_target(parsed)
        build = get_build(parsed, deployment,target. build_types)
        build.execute_target(target, context=Path(parsed.path))