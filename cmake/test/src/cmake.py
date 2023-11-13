"""
cmake.py:

A wrapper used to call CMake given a directory, a target directory to build, and a list of expected
outputs for the given run. Options to the run and build targets (make targets) can optionally be
supplied. CMake is run as a command line call to the cmake system. Thus CMake must be installed.

@author mstarch
"""
import multiprocessing
import select
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

import pytest

# Constants to supplied to the calls to subprocess
CPUS = multiprocessing.cpu_count()
CMAKE = "cmake"
MAKE_CALL = "make"
MAKE_ARGS = [f"-j{CPUS}"]


def subprocess_helper(args, cwd):
    """Subprocess helper used to 'tee' the output to: console and capture"""

    def read_available(proc, stdout, stderr):
        lines = {stdout: [], stderr: []}
        while proc.poll() is None:
            ready, _, _ = select.select([stdout, stderr], [], [])
            for stream in ready:
                line = stream.readline()
                lines[stream].append(line)
                if "-s" in sys.argv or "--capture=no" in sys.argv:
                    print(
                        line,
                        end="",
                        file=(sys.stdout if stream == stdout else sys.stderr),
                    )
        lines[stdout].extend(stdout.readlines())
        lines[stderr].extend(stderr.readlines())
        return (
            proc.poll(),
            [line for line in lines[stdout] if line.strip() != ""],
            [line for line in lines[stderr] if line.strip() != ""],
        )

    # Verbose output desired from pytest
    proc = subprocess.Popen(
        args=args,
        cwd=str(cwd),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    return read_available(proc, proc.stdout, proc.stderr)


def run_cmake(source_directory, build_path, options=None):
    """
    Runs the cmake in the current directory with the given options and build_path

    :param source_directory: source directory to run CMake within
    :param build_path: path to build
    :param options: options to pass CMake
    :return: True if successful, False otherwise
    """
    args = [CMAKE]
    options = {} if options is None else options
    for option in options.keys():
        value = options[option]
        args.append("-D{0}={1}".format(option, value))
    args.append(source_directory)

    return subprocess_helper(args, build_path)


def run_make(build_directory, target):
    """
    Runs the make command to ensure that the CMake system can follow through and finish the build.
    Note: this assumes that the provided application built properly. Thus, those unit tests should
    run first.

    :param build_directory: build directory to change to
    :param target: target to the make command
    :return: True if successful, False otherwise
    """
    args = [MAKE_CALL]
    if target != "":
        args.append(target)
    args.extend(MAKE_ARGS)
    return subprocess_helper(args, build_directory)


def assert_process_success(data_object, errors_ok=False):
    """Assert the subprocess runs worked as expected"""
    for field in ["source", "build", "install", "cmake", "targets"]:
        assert field in data_object, f"Data object malformed: missing '{field}' field"
        assert field in data_object, f"Data object malformed: missing '{field}' field"

    return_code, stdout, stderr = data_object["cmake"]
    assert return_code == 0, f"CMake generation failed with return code {return_code}"
    assert stdout, "CMake generated no standard out process"
    assert not stderr or errors_ok, f"CMake generated errors:\n{''.join(stderr)}"

    for target, output in data_object["targets"].items():
        return_code, stdout, stderr = output
        assert return_code == 0, f"CMake failed building '{target}'"
        assert stdout, "CMake generated no standard out building '{target}'"


def get_build(
    fixture_name,
    source_directory,
    cmake_arguments=None,
    make_targets=None,
    install_directory=None,
):
    """Generate and build a cmake deployment, then returns a pytest fixture for it"""
    base_cmake_arguments = {"FPRIME_SUB_BUILD_JOBS": f"{CPUS}"}
    cmake_arguments = {} if cmake_arguments is None else cmake_arguments
    cmake_arguments.update(base_cmake_arguments)
    build_directory = Path(tempfile.mkdtemp())
    install_directory_calc = Path(
        Path(source_directory) / "build-artifacts"
        if install_directory is None
        else install_directory
    )
    make_targets = ["all"] if make_targets is None else make_targets
    if install_directory is not None:
        cmake_arguments["CMAKE_INSTALL_PREFIX"] = str(install_directory)

    cmake_output = run_cmake(source_directory, build_directory, cmake_arguments)
    target_outputs = {
        target: run_make(build_directory, target) for target in make_targets
    }

    @pytest.fixture(scope="session", name=fixture_name)
    def fixture_function():
        yield {
            "source": source_directory,
            "build": build_directory,
            "install": install_directory_calc,
            "cmake": cmake_output,
            "targets": target_outputs,
        }
        shutil.rmtree(build_directory, ignore_errors=True)
        shutil.rmtree(install_directory, ignore_errors=True)

    return fixture_function
