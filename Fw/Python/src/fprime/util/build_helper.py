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
import re
import sys
from pathlib import Path
from typing import Dict, List, Tuple

from fprime.common.error import FprimeException
from fprime.fbuild.builder import (
    Target,
    Build,
    BuildType,
    GenerateException,
    InvalidBuildCacheException,
    UnableToDetectDeploymentException,
)


CMAKE_REG = re.compile(r"-D([a-zA-Z0-9_]+)=(.*)")


def get_target(parsed: argparse.Namespace) -> Target:
    """Gets the target given the argparse namespace

    Takes the parsed namespace and processes it to a known matching target.

    Args:
        parsed: argparse namespace to read values from

    Returns:
        target that can support the given supplied namespace
    """
    mnemonic = parsed.command
    flags = {
        flag for flag in Target.get_all_possible_flags() if getattr(parsed, flag, False)
    }
    return Target.get_target(mnemonic, flags)


def validate(parsed, unknown):
    """
    Validate rules to ensure that the args are properly consistent. This will also generate a set of validated arguments
    to pass to CMake. This allows these values to be created, defaulted, and validated in one place
    :param parsed: args to validate
    :param unknown: unknown arguments
    :return: cmake arguments to pass to CMake
    """
    cmake_args = {}
    make_args = {}
    # Check platforms for existing toolchain, unless the default is specified.
    if parsed.command == "generate":
        d_args = {
            match.group(1): match.group(2)
            for match in [CMAKE_REG.match(arg) for arg in unknown]
        }
        cmake_args.update(d_args)
    # Build type only for generate, jobs only for non-generate
    elif (
        parsed.command != "info"
        and parsed.command != "purge"
        and parsed.command != "hash-to-file"
    ):
        parsed.settings = None  # Force to load from cache if possible
        make_args.update({"--jobs": (1 if parsed.jobs <= 0 else parsed.jobs)})
    return cmake_args, make_args


def add_target_parser(
    target: Target,
    subparsers,
    common: argparse.ArgumentParser,
    existing: Dict[str, Tuple[argparse.ArgumentParser, List[str]]],
):
    """Add a subparser for a given build target

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
        parser = subparsers.add_parser(
            target.mnemonic,
            parents=[common],
            add_help=False,
            help="{} in the specified directory".format(target.desc),
        )
        # --ut flag also exists at the global parsers, skip adding it
        existing[target.mnemonic] = (parser, ["ut"])
        # Common target-only items
        parser.add_argument(
            "-j",
            "--jobs",
            default=1,
            type=int,
            help="Parallel build job count. Default: %(default)s.",
        )
    parser, flags = existing[target.mnemonic]
    new_flags = [flag for flag in target.flags if flag not in flags]
    for flag in new_flags:
        parser.add_argument(
            "--{}".format(flag), action="store_true", default=False, help=target.desc
        )
    flags.extend(new_flags)


def parse_args(args):
    """
    Parse the arguments to the CLI. This will then enable the user to run the above listed commands via the commands.
    :param args: CLI arguments to process
    :return: parsed arguments in a Namespace
    """
    # Common parser specifying common arguments input into the utility
    common_parser = argparse.ArgumentParser(
        description="Common Parser for Common Ingredients."
    )
    common_parser.add_argument(
        "platform",
        nargs="?",
        default="default",
        help="F prime build platform (e.g. Linux, Darwin). Default specified in settings.ini",
    )
    common_parser.add_argument(
        "-d",
        "--deploy",
        dest="deploy",
        default=None,
        help="F prime deployment directory to use. May contain multiple build directories.",
    )
    common_parser.add_argument(
        "-p",
        "--path",
        default=os.getcwd(),
        help="F prime directory to operate on. Default: cwd, %(default)s.",
    )
    common_parser.add_argument(
        "-v",
        "--verbose",
        default=False,
        action="store_true",
        help="Turn on verbose output.",
    )
    common_parser.add_argument(
        "--ut",
        action="store_true",
        help="Run command against unit testing build type",
    )

    # Main parser for the whole application
    parsers = {}
    parser = argparse.ArgumentParser(description="F prime helper application.")
    subparsers = parser.add_subparsers(
        description="F prime utility command line. Please run one of the commands. "
        + "For help, run a command with the --help flag.",
        dest="command",
    )
    # Add non-target parsers
    generate_parser = subparsers.add_parser(
        "generate",
        help="Generate a build cache directory. Defaults to generating a release build cache",
        parents=[common_parser],
        add_help=False,
    )
    generate_parser.add_argument(
        "-Dxyz",
        action="append",
        help="Pass -D flags through to CMakes",
        nargs=1,
        default=[],
    )
    purge_parser = subparsers.add_parser(
        "purge",
        help="Purge build cache directories",
        add_help=False,
        parents=[common_parser],
    )
    purge_parser.add_argument(
        "-f",
        "--force",
        default=False,
        action="store_true",
        help="Purges the build directory by force. No confirmation will be requested.",
    )
    # Add a search for hash function
    hash_parser = subparsers.add_parser(
        "hash-to-file",
        help="Converts F prime build hash to filename.",
        parents=[common_parser],
        add_help=False,
    )
    hash_parser.add_argument(
        "hash",
        type=lambda x: int(x, 0),
        help="F prime assert hash to associate with a file.",
    )

    # Add a search for hash function
    subparsers.add_parser(
        "info",
        help="Gets fprime-util contextual information.",
        parents=[common_parser],
        add_help=False,
    )
    for target in Target.get_all_targets():
        add_target_parser(target, subparsers, common_parser, parsers)
    # Parse and prepare to run
    parsed, unknown = parser.parse_known_args(args)
    bad = [bad for bad in unknown if not CMAKE_REG.match(bad)]
    if not hasattr(parsed, "command") or parsed.command is None:
        parser.print_help()
        sys.exit(1)
    elif bad:
        print("[ERROR] Unknown arguments: {}".format(", ".join(bad)))
        parser.print_help()
        sys.exit(1)
    cmake_args, make_args = validate(parsed, unknown)
    return parsed, cmake_args, make_args, parser


def confirm():
    """ Confirms the removal of the file with a yes or no input """
    # Loop "forever" intended
    while True:
        confirm_input = input("Purge this directory (yes/no)?")
        if confirm_input.lower() in ["y", "yes"]:
            return True
        if confirm_input.lower() in ["n", "no"]:
            return False
        print("{} is invalid.  Please use 'yes' or 'no'".format(confirm_input))


def print_info(parsed, deployment):
    """ Builds and prints the informational output block """
    cwd = Path(parsed.path)
    build_types = BuildType

    # Roll up targets for more concise display
    build_infos = {}
    local_generic_targets = set()
    global_generic_targets = set()
    # Loop through available builds and harvest targets
    for build_type in build_types:
        build = Build(build_type, deployment, verbose=parsed.verbose)
        try:
            build.load(cwd, parsed.platform)
        except InvalidBuildCacheException:
            print(
                "[WARNING] Not displaying results for build type '{}', missing build cache.".format(
                    build_type.get_cmake_build_type()
                )
            )
            continue
        build_info = build.get_build_info(cwd)
        # Target list
        local_targets = {
            "'{}'".format(target) for target in build_info.get("local_targets", [])
        }
        global_targets = {
            "'{}'".format(target) for target in build_info.get("global_targets", [])
        }
        build_artifacts = (
            build_info.get("auto_location")
            if build_info.get("auto_location") is not None
            else "N/A"
        )
        local_generic_targets = local_generic_targets.union(local_targets)
        global_generic_targets = global_generic_targets.union(global_targets)
        build_infos[build_type] = build_artifacts

    # Print out directory and deployment target sections
    print("[INFO] Fprime build information:")
    print("    Available directory targets: {}".format(" ".join(local_generic_targets)))
    print()
    print(
        "    Available deployment targets: {}".format(" ".join(global_generic_targets))
    )

    # Artifact locations come afterwards
    print("  ----------------------------------------------------------")
    for build_type, build_artifact_location in build_infos.items():
        format_string = "    {} build cache: {}"
        print(
            format_string.format(
                build_type.get_cmake_build_type(), build_artifact_location
            )
        )
    print()


def print_hash_info(lines, hash_val):
    """ Prints out hash info from lines """
    # Print out lines when found
    if lines:
        print("[INFO] File(s) associated with hash 0x{:x}".format(hash_val))
        for line in lines:
            print("   ", line, end="")
        return
    print(
        "[ERROR] No file hashes found in {} build. Do you need '--build-type Testing' for a unittest run?"
    )


def utility_entry(args):
    """ Main interface to F prime utility """
    parsed, cmake_args, make_args, parser = parse_args(args)
    cwd = Path(parsed.path)

    build_type = BuildType.BUILD_NORMAL
    if parsed.ut:
        build_type = BuildType.BUILD_TESTING

    try:
        deployment = (
            Path(parsed.deploy)
            if parsed.deploy is not None
            else Build.find_nearest_deployment(cwd)
        )
        if parsed.command is None:
            print("[ERROR] Must supply subcommand for fprime-util. See below.")
            parser.print_help()
            print_info(parsed, deployment)
        elif parsed.command == "info":
            print_info(parsed, deployment)
        elif parsed.command == "hash-to-file":
            build = Build(build_type, deployment, verbose=parsed.verbose)
            lines = build.find_hashed_file(parsed.hash)
            print_hash_info(lines, parsed.hash)
        elif parsed.command == "generate":
            build = Build(build_type, deployment, verbose=parsed.verbose)
            build.invent(cwd, parsed.platform)
            toolchain = build.find_toolchain()
            print("[INFO] Generating build directory at: {}".format(build.build_dir))
            print(
                "[INFO] Using toolchain file {} for platform {}".format(
                    toolchain, parsed.platform
                )
            )
            if toolchain is not None:
                cmake_args.update({"CMAKE_TOOLCHAIN_FILE": toolchain})
            build.generate(cmake_args)
        elif parsed.command == "purge":
            for build_type in BuildType:
                build = Build(build_type, deployment, verbose=parsed.verbose)
                try:
                    build.load(cwd, parsed.platform)
                except InvalidBuildCacheException:
                    continue
                print(
                    "[INFO] {} build directory at: {}".format(
                        parsed.command.title(), build.build_dir
                    )
                )
                if parsed.force or confirm():
                    build.purge()

            build = Build(BuildType.BUILD_NORMAL, deployment, verbose=parsed.verbose)
            try:
                build.load(cwd, parsed.platform)
            except InvalidBuildCacheException:
                # Just load the install destination regardless of whether the build is valid.
                pass

            install_dir = build.install_dest_exists()
            if install_dir is None:
                return

            print(
                "[INFO] {} install directory at: {}".format(
                    parsed.command.title(), install_dir
                )
            )
            if parsed.force or confirm():
                build.purge_install()
        else:
            target = get_target(parsed)
            build = Build(target.build_type, deployment, verbose=parsed.verbose)
            build.load(cwd, parsed.platform)
            build.execute(target, context=Path(parsed.path), make_args=make_args)
    except GenerateException as genex:
        print(
            "[ERROR] {}. Partial build cache remains. Run purge to clean-up.".format(
                genex
            ),
            file=sys.stderr,
        )
    except UnableToDetectDeploymentException:
        print(
            "[ERROR] Could not detect deployment directory for: {}".format(parsed.path)
        )
    except FprimeException as exc:
        print("[ERROR] {}".format(exc), file=sys.stderr)
        return 1
    return 0
