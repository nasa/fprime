"""
(test) fprime.build:

Tests the current F prime build module. Ensures that the selected singleton defines the minimum set of functions and
that they function as expected.

@author mstarch
"""
import os
import shutil
import tempfile
import pathlib

import pytest

import fprime.fbuild.cmake
import fprime.fbuild.builder


def get_cmake_builder():
    """Gets a CMake builder for these tests

    Returns:
        CMakeBuilder for testing
    """
    return fprime.fbuild.cmake.CMakeHandler()


def get_data_dir():
    """
    Gets directory containing test-data specific to the builder being tested. This will enable new implementors, should
    there be any, to implement their own build-directory structure.

    :return:
    """
    if type(get_cmake_builder()) == fprime.fbuild.cmake.CMakeHandler:
        return os.path.join(os.path.dirname(__file__), "cmake-data")
    raise Exception(
        "Test data directory not setup for {} builder class".format(
            type(get_cmake_builder())
        )
    )


def test_hash_finder():
    """
    Tests that the hash finder works given a known builds.
    """
    local = pathlib.Path(os.path.dirname(__file__))
    dep_dir = local / "cmake-data" / "testbuild"
    builder = fprime.fbuild.builder.Build(
        fprime.fbuild.builder.BuildType.BUILD_NORMAL, dep_dir
    )
    builder.load(build_dir=dep_dir / "build-fprime-automatic-native")

    assert builder.find_hashed_file(0xDEADBEEF) == ["Abc: 0xdeadbeef\n"]
    assert builder.find_hashed_file(0xC0DEC0DE) == ["HJK: 0xc0dec0de\n"]


def test_needed_functions():
    """
    Test the needed functions for the given builder. This will ensure that the public interface to the builder is
    implemented as expected.
    """
    needed_funcs = [
        "get_include_info",
        "execute_known_target",
        "get_include_locations",
        "get_fprime_configuration",
    ]
    for func in needed_funcs:
        assert hasattr(get_cmake_builder(), func)


def test_get_fprime_configuration():
    """
    Tests the given fprime configuration fetcher. Required for other portion of the system.
    """
    configs = fprime.fbuild.cmake.CMakeHandler.CMAKE_LOCATION_FIELDS
    test_data = {
        "grand-unified": (
            "/home/user11/fprime/Ref/..",
            None,
            "/home/user11/fprime/Ref/..",
        ),
        "subdir": (
            "/home/user11/Proj",
            "/home/user11/Proj/lib1;/home/user11/Proj/lib2",
            "/home/user11/Proj/fprime",
        ),
        "external": ("/home/user11/Proj", "/opt/lib1;/opt/lib2", "/opt/fprime"),
    }
    for key in test_data.keys():
        build_dir = os.path.join(get_data_dir(), key)
        # Test all path, truth pairs
        values = get_cmake_builder().get_fprime_configuration(configs, build_dir)
        assert values == test_data[key]


def test_get_include_locations():
    """
    Test all the include locations. This will ensure that values are properly read from a cache listing. This will
    support various other portions of the system, so debug here firest.
    """
    test_data = {
        "grand-unified": ["/home/user11/fprime"],
        "subdir": [
            "/home/user11/Proj",
            "/home/user11/Proj/lib1",
            "/home/user11/Proj/lib2",
            "/home/user11/Proj/fprime",
        ],
        "external": ["/home/user11/Proj", "/opt/lib1", "/opt/lib2", "/opt/fprime"],
    }
    for key in test_data.keys():
        build_dir = os.path.join(get_data_dir(), key)
        paths = list(get_cmake_builder().get_include_locations(build_dir))
        assert paths == test_data[key]


def test_get_include_info():
    """
    Tests that the include root function gets the expected value based on the path and build-directory setups.
    """
    # Test data setup, format build_dir to tuples of path, expected result. Note: None means expect Orphan exception
    test_data = {
        "grand-unified": [
            (
                "/home/user11/fprime/Svc/SomeComp1",
                ("Svc/SomeComp1", "/home/user11/fprime"),
            ),
            (
                "/home/user11/fprime/Ref/SomeComp2",
                ("Ref/SomeComp2", "/home/user11/fprime"),
            ),
            (
                "/home/user11/fprime/Ref/SomeComp2/../SomeComp1",
                ("Ref/SomeComp1", "/home/user11/fprime"),
            ),
            ("/home/user11/external-sw/NachoDeploy/SomeComp3", None),
        ],
        "subdir": [
            (
                "/home/user11/Proj/fprime/Svc/SomeComp1",
                ("Svc/SomeComp1", "/home/user11/Proj/fprime"),
            ),
            # An insidious case where the path contains a common prefix as another possible location without being an
            # exact directory.  Notice /home/user11/Proj/fprime-something/Comps/SomeComp1 contains a non-exact common
            # prefix with the directory /home/user11/Proj/fprime. i.e. /home/user11/Proj/fprime is a prefix but
            # /home/user11/Proj/fprime/ is not.
            (
                "/home/user11/Proj/fprime-something/Comps/SomeComp1",
                ("fprime-something/Comps/SomeComp1", "/home/user11/Proj"),
            ),
            ("/home/user11/Proj/Ref/SomeComp2", ("Ref/SomeComp2", "/home/user11/Proj")),
            (
                "/home/user11/Proj/Ref/SomeComp2/../../fprime/Svc/SomeComp1",
                ("Svc/SomeComp1", "/home/user11/Proj/fprime"),
            ),
            ("/home/user11/external-sw/NachoDeploy/SomeComp3", None),
        ],
        "external": [
            ("/opt/fprime/Svc/SomeComp1", ("Svc/SomeComp1", "/opt/fprime")),
            ("/home/user11/Proj/Ref/SomeComp2", ("Ref/SomeComp2", "/home/user11/Proj")),
            ("/opt/something/else/external-sw/NachoDeploy/SomeComp3", None),
        ],
    }
    # Run through all the above data look for matching ansers
    for key in test_data.keys():
        build_dir = os.path.join(get_data_dir(), key)
        # Test all path, truth pairs
        for path, truth in test_data.get(key):
            if truth is None:
                with pytest.raises(fprime.fbuild.cmake.CMakeOrphanException):
                    value = get_cmake_builder().get_include_info(path, build_dir)
            else:
                value = get_cmake_builder().get_include_info(path, build_dir)
                assert value == truth


def test_find_nearest_deployment():
    """
    This will test the ability for the system to detect valid deployment directories
    """

    test_dir = pathlib.Path(get_data_dir())
    test_data = [
        ("testbuild/subdir1/subdir2/subdir3", "testbuild/subdir1/"),
        ("testbuild/subdir1/subdir2", "testbuild/subdir1/"),
        ("testbuild/", "testbuild/"),
        ("/nonexistent/dirone/someotherpath", None),
    ]
    for path, truth in test_data:
        path = test_dir / path
        if truth is not None:
            truth = test_dir / truth
            value = fprime.fbuild.builder.Build.find_nearest_deployment(path)
            assert value == truth
        else:
            with pytest.raises(fprime.fbuild.builder.UnableToDetectDeploymentException):
                fprime.fbuild.builder.Build.find_nearest_deployment(path)


def test_generate():
    """
    Generate a directory and ensure that it works via CMake. This tests the common setup flags for normal and testing
    builds. Will also test the right errors are raised.
    """
    rms = []
    try:
        test_flags = [{}, {"CMAKE_BUILD_TYPE": "Testing"}]
        # Build Ref with flags
        path = os.path.join(
            os.path.dirname(__file__), "..", "..", "..", "..", "..", "Ref"
        )
        for flags in test_flags:
            # Create a temp directory and register its deletion at the end of the program run
            tempdir = tempfile.mkdtemp()
            rms.append(tempdir)
            get_cmake_builder().generate_build(path, tempdir, flags, ignore_output=True)
        # Expect errors for this step
        path = "/nopath/somesuch/nothing"
        with pytest.raises(fprime.fbuild.cmake.CMakeProjectException):
            tempdir = tempfile.mkdtemp()
            rms.append(tempdir)
            get_cmake_builder().generate_build(path, tempdir, ignore_output=True)
    # Clean-Up all the directories made
    finally:
        for rmd in rms:
            shutil.rmtree(rmd, ignore_errors=True)


def test_targets():
    """
    Test standard targets for the build.
    """
    # Build Ref with flags
    tempdir = None
    try:
        fprime_root = os.path.join(
            os.path.dirname(__file__), "..", "..", "..", "..", ".."
        )
        # Create a temp directory and register its deletion at the end of the program run
        tempdir = tempfile.mkdtemp()
        get_cmake_builder().generate_build(
            os.path.join(fprime_root, "Ref"), tempdir, {"CMAKE_BUILD_TYPE": "Testing"}
        )
        test_data = [
            (os.path.join(fprime_root, "Ref"), ""),
            (os.path.join(fprime_root, "Svc", "CmdDispatcher"), ""),
            (os.path.join(fprime_root, "Svc", "CmdDispatcher"), "ut_exe"),
            (os.path.join(fprime_root, "Svc", "CmdDispatcher"), "check"),
        ]
        # Loop over all directories and target pairs ensuing things work
        for path, target in test_data:
            get_cmake_builder().execute_known_target(target, tempdir, path)
        test_data = [
            (os.path.join(fprime_root, "Svc", "CmdDispatcher"), "nontarget1"),
            (os.path.join(fprime_root, "Svc", "CmdDispatcher3Not"), ""),
        ]
        # Loop over all paths and target pairs looking for expected Exceptions
        for path, target in test_data:
            with pytest.raises(fprime.fbuild.cmake.CMakeException):
                get_cmake_builder().execute_known_target(target, tempdir, path)
    # Clean up when all done
    finally:
        if tempdir is not None:
            shutil.rmtree(tempdir, ignore_errors=True)
