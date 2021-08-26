"""
cmake.py:

A wrapper used to call CMake given a directory, a target directory to build, and a list of expected
outputs for the given run. Options to the run and build targets (make targets) can optionally be
supplied. CMake is run as a command line call to the cmake system. Thus CMake must be installed.

@author mstarch
"""
import os
import sys
import shutil
import time
import tempfile
import subprocess
import functools
import platform

# Constants to supplied to the calls to subprocess
CMAKE = "cmake"
MAKE_CALL = "make"
MAKE_ARGS = ["-j2"]


def register_test(
    module, name, build_dir=None, options=None, expected=None, targets=None
):
    """
    Registers a test to the given module using the given name. This will create a function of the
    form test_<name> registered to the module ready for autodetect.

    :param module: name of module for registration
    :param name: name to register test with
    """
    module = sys.modules[module]
    if options is None:
        options = getattr(module, "OPTIONS", {})
    if expected is None:
        expected = getattr(module, "EXPECTED", [])
    if build_dir is None:
        build_dir = getattr(module, "BUILD_DIR")
    if targets is None:
        targets = getattr(module, "TARGETS", [""])
    name = "test_{0}".format(name.replace("/", "_").replace("-", "_").replace(" ", "_"))
    setattr(
        module,
        name,
        functools.partial(
            run_build, build_dir, expected, make_targets=targets, options=options
        ),
    )


def run_cmake(build_path, options={}, capout=False):
    """
    Runs the cmake in the current directory with the given options and build_path

    :param build_path: path to build
    :param options: options to pass CMake
    :return: True if successful, False otherwise
    """
    keys = {}
    args = [CMAKE]
    for option in options.keys():
        value = options[option]
        args.append("-D{0}={1}".format(option, value))
    args.append(build_path)
    if capout:
        keys["stdout"] = subprocess.PIPE
        keys["stderr"] = subprocess.PIPE
    # If the output is not sent into the terminal, route the output to no-op discarder or the
    # process will block on pytests' inability to handle all the output silently
    elif not "-s" in sys.argv and not "--capture=no" in sys.argv:
        keys["stdout"] = subprocess.DEVNULL
        keys["stderr"] = subprocess.DEVNULL
    print("Running:", args, "With args:", keys)
    proc = subprocess.Popen(args, **keys)
    if capout:
        stdout, stderr = proc.communicate()
        return (proc.pid == 0, stdout, stderr)
    return proc.wait() == 0


def run_make(target):
    """
    Runs the make command to ensure that the CMake system can follow through and finish the build.
    Note: this assumes that the provided application build properly. Thus, those unit tests should
    run first.

    :param target: target to the make command
    :return: True if successful, False otherwise
    """
    args = [MAKE_CALL]
    if target != "":
        args.append(target)
    args.extend(MAKE_ARGS)
    proc = subprocess.Popen(args)
    return proc.wait() == 0


def assert_exists(expected, install_dest, start_time):
    """
    Goes through all of the expected files and check to ensure that each is a file. This also
    ensures that the file is new enough to have been generated at the start of this build.

    :param expected: list of expected files. Relative to build. <FPRIME> replaced with f prime dir.
    :param start_time: time of the start of this build
    """
    for expect in expected:
        expect = expect.replace(
            "<FPRIME>", os.path.join(os.path.dirname(__file__), "..", "..", "..")
        )
        expect = expect.replace(
            "<FPRIME_INSTALL>", os.path.join(install_dest, platform.system())
        )
        assert os.path.exists(expect), "CMake build failed to generate '{0}'".format(
            expect
        )
        assert (
            os.path.getmtime(expect) >= start_time
        ), "CMake did not recreate/update '{0}'".format(expect)


def run_build(
    build_path, expected_outputs, build_directory=None, make_targets=[""], options={}
):
    """
    Run the CMake command, and any number of make commands. Ensures that the calls all process as
    expected, and that the expected outputs are produced.

    :param build_path: target path. Usually <FPRIME>/Ref. <FPRIME> replaced with f prime dir.
    :param expected_outputs: list of files to check for. Relative to build directory.
    :param build_directory: directory in which to perform the build, default is a temporary dir.
    :param make_targets: list of make targets to run.
    :param options: options to supply to the cmake system.
    :return: True if the test passes, False if there is an error
    """
    start_time = time.time()
    if build_directory is not None:
        raise Exception("Named build directories not supported, yet.")
    owd = os.getcwd()
    tmpd = tempfile.mkdtemp()

    if not "FPRIME_INSTALL_DEST" in options:
        options["FPRIME_INSTALL_DEST"] = os.path.join(tmpd, "build-artifacts")

    try:
        os.chdir(tmpd)
        build_path = build_path.replace(
            "<FPRIME>", os.path.join(os.path.dirname(__file__), "..", "..", "..")
        )
        assert run_cmake(
            build_path, options=options
        ), "CMake failed for path '{0}' and options '{1}'".format(build_path, options)
        for target in make_targets:
            assert run_make(
                target
            ), "Failed to build '{0}' target for '{1}' and options '{2}'".format(
                target, build_path, options
            )
        assert_exists(expected_outputs, options["FPRIME_INSTALL_DEST"], start_time)
    finally:
        os.chdir(owd)
        shutil.rmtree(tmpd, ignore_errors=False)
