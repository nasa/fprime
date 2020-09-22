"""
Supplies high-level build functions to the greater fprime helper CLI. This maps from user command space to the specific
build system handler underneath.
"""
import os
import re
import functools
from abc import ABC
from enum import Enum
from pathlib import Path
from typing import Iterable, List, Set, Union

from fprime.common.error import FprimeException
from fprime.fbuild.settings import IniSettings
from fprime.fbuild.cmake import CMakeHandler, CMakeException


class BuildType(Enum):
    """
    An enumeration used to represent the various build types used to build fprime. These types can support different
    types of targets underneath. i.e. the unit-test build may build unit test executables.
    """

    """ Normal build normal binaries for a deployment mapping to CMake 'Release'""" # pylint: disable=W0105
    BUILD_NORMAL = 0,
    """ Testing build allowing unit testing mapping to CMake 'Testing'""" # pylint: disable=W0105
    BUILD_TESTING = 1

    def get_suffix(self):
        """ Get the suffix of a directory supporting this build """
        if self == BuildType.BUILD_NORMAL:
            return ""
        elif self == BuildType.BUILD_TESTING:
            return "-ut"
        assert False, "Invalid build type"

    def get_cmake_build_type(self):
        """ Get the suffix of a directory supporting this build """
        if self == BuildType.BUILD_NORMAL:
            return "Release"
        elif self == BuildType.BUILD_TESTING:
            return "Testing"
        assert False, "Invalid build type"


class Target(ABC):
    """Generic build target base class

    A target can be specified by the user using a mnemonic and flags. The mnemonic is the command typed in by the user,
    and the flags allow the user to remember fewer mnemonics by changing the build target using a modifier. Each build
    target is available in certain build types.

    Targets can be global, using the GlobalTarget base class. Global targets don't use contextual information to modify
    the target, but apply to the whole deployment. Note: global targets are also engaged at the deployment level should
    that be the context.

    Targets may also be local. These targets use context information to figure out what to build. This allows for one
    target to represent a class of targets. i.e. build can be used as a local target to build any given sub directory.
    """

    def __init__(self, mnemonic:str, desc:str, build_types:List[BuildType] = None, flags:set = None, cmake:str = None):
        """ Constructs a build target

        Args:
            mnemonic:    mnemonic used to engage build targets. Is not unique, but mnemonic + flags must be.
            desc:        help description of this build target
            build_types: supported build types for target. Defaults to [BuildType.BUILD_NORMAL, BuildType.BUILD_TESTING]
            flags:       flags used to uniquely identify build targets who share logical mnemonics. Defaults to None.
            cmake:       cmake target override to handle oddly named cmake targets
        """
        self.mnemonic = mnemonic
        self.desc = desc
        self.build_types = build_types if build_types is not None else [BuildType.BUILD_NORMAL, BuildType.BUILD_TESTING]
        self.flags = flags if flags is not None else set()
        self.cmake_target = cmake if cmake is not None else mnemonic

    def __str__(self):
        """ Makes this target into a string """
        return self.config_string(self.mnemonic, self.flags)

    @staticmethod
    def config_string(mnemonic, flags):
        """Converts a mnemonic and set of flags to string

        Args:
            mnemonic: mnemonic of the target
            flags: sset of flags to pair with mnemonic
        Returns:
            string of format "mnemonic --flag1 --flag2 ..."
        """
        flag_string = " ".join(["--{}".format(flag) for flag in flags])
        flag_string = "" if flag_string == "" else " " + flag_string
        return "{}{}".format(mnemonic, flag_string)

    @classmethod
    def get_all_possible_flags(cls) -> Set[str]:
        """ Gets list of all targets' flags used

        Returns:
            List of targets's supported by the system
        """
        return functools.reduce(lambda agg, item: agg.union(item.flags), cls.get_all_targets(), set())

    @classmethod
    def get_all_targets(cls) -> List['Target']:
        """ Gets list of all targets registered

        Returns:
            List of targets supported by the system
        """
        return BUILD_TARGETS

    @classmethod
    def get_target(cls, mnemonic:str, flags: Set[str]) -> 'Target':
        """ Gets the actual build target given the parsed namespace

        Using the global list of build targets and the flags supplied to the namespace, attempt to determine which build
        targets can be used. If more than one are found, then generate exception.

        Args:
            mnemonic: mnemonic of command to look for
            flags:    flags to narrow down target

        Returns:
            single matching target
        """
        #matching = [target for target in cls.get_all_targets() if target.mnemonic == mnemonic and flags == target.flags]
        matching = []
        for target in cls.get_all_targets():
            if target.mnemonic == mnemonic and flags == target.flags:
                matching.append(target)
        if not matching:
            raise NoSuchTargetExcetion("Could not find target '{}'".format(cls.config_string(mnemonic, flags)))
        assert len(matching) == 1, "Conflicting targets specified in code"
        return matching[0]


class GlobalTarget(Target):
    """Represents a global build target

    Build targets are global if they do not apply to a specific directory, but rather to a full deployment.
    """


class LocalTarget(Target):
    """Represents a local build target

    Build targets are local if they do  apply to a specific directory whose context drives the build setup.
    """


class Build:
    """Represents a build configuration

    Builds in F´ consist of a build type (normal, testing), a deployment directory, a set of settings, and a target
    platform. These are tracked as part of this Build class. This helps setup a build cache directory, load default
    settings, and track what type of build is being run.

    BuildType represents the type of build as explained in that enum type.
    Deployments are an individual build of fprime, and should define the CMakeLists.txt file as a child of this
    directory. A default settings.ini file may be found here.
    Platforms represent the target hardware to build from. This is translated to the CMake toolchain file.

    After creation, a user must use invent to handle new builds (e.g. during the generation step), or load to load a
    previously generated build.

    Examples:
        To use in generation run the following code.

        build = Build(BuildType.BUILD_NORMAL, path/to/deployment)
        build.invent("raspberrypi")

        To use at any step after generation:

        build = Build(BuildType.BUILD_NORMAL, path/to/deployment)
        build.load()
    """
    VALID_CMAKE_LIST = re.compile(r"(?sm)\sproject\(.*\)")
    CMAKE_DEFAULT_BUILD_NAME = "build-fprime-automatic-{platform}{suffix}"

    def __init__(self, build_type: BuildType, deployment: Path, verbose:bool = False):
        """ Constructs a build object from its constituent parts

        Args:
            build_type: member of the enum BuildType specifying fprime build type
            deployment: path to deployment that this build represents
        """
        self.build_type = build_type
        self.deployment = deployment
        self.settings = None
        self.platform = None
        self.build_dir = None
        self.cmake = CMakeHandler()
        self.cmake.set_verbose(verbose)

    def invent(self, platform: str = None, build_dir: Path = None):
        """ Invents a build path from a given platform

        Sets this build up as a new build that would be used as as part of a generate step. This directory must not
        already exist. If platform is None, a default will be chosen from the settings.ini file. If the settings.ini
        file does not exist, or does not specify a default_toolchain, then "native" will be used. Settings are loaded in
        this step for further uses of this build.

        build_dir is used to specify an exact build directory to use as part of this step. This allows directories to be
        specified by the caller, but is typically not used.

        Args:
            platform:   name of platform to build against. None will use default from settings.ini or without this
                        setting, "native". Defaults to None.
            build_dir:  explicitly sets the build path to allow for user override of default

        Raises:
            InvalidBuildCacheException: a build cache already exists as it should not
        """
        self.__setup_default()
        if self.build_dir.exists():
            raise InvalidBuildCacheException("{} already exists.".format(self.build_dir))

    def load(self, platform: str = None, build_dir: Path = None):
        """ Load an existing build cache

        Sets this build up from an existing build cache. This can be used after a previous run that has generated a
        build cache in order to prepare for other build steps.

        Args:
            platform:   name of platform to build against. None will use default from settings.ini or without this
                        setting, "native". Defaults to None.
            build_dir:  explicitly sets the build path to allow for user override of default

        Raises:
            InvalidBuildCacheException: the build cache does not exist as it must
        """
        self.__setup_default()
        if not self.build_dir.exists() or not (self.build_dir / "CMakeCache.txt").exists():
            raise InvalidBuildCacheException("{} invalid build cache. Please (re)generate.".format(build_dir))

    def get_settings(self, setting: Union[str, Iterable[str]], default: Union[str, Iterable[str]]) -> Union[str, Iterable[str]]:
        """ Fetches settings in the settings file

        Reads settings loaded from the settings file and returns them to the caller. If a single string is submitted,
        then a single string is returned. If a list of strings is submitted a list is returned. default provides default
        values to supply in the case that a setting is unavailable.

        Args:
            setting: a string or set of string settings to return
            default: a string or set of string settings to return if no setting is found

        Returns:
            a single string setting or a list of string settings to match request with defaults subbed ins
        """
        if isinstance(setting, str):
            return self.settings.get(setting, default)
        return [self.get_settings(req, back) for req, back in zip(setting, default)]

    def find_hashed_file(self, hash_value: int) -> List[str]:
        """ Retrieves the file associated with a hash

        In order to reduce space and memory footprint, filenames are associated with hashes automatically as part of the
        build. This function will retrieve the file name given a has integer.

        Args:
            hash_value: hash number to lookup

        Returns:
            stored file path(s) associated with hash
        """
        hashes_file = self.build_dir / "hashes.txt"
        if not hashes_file.exists():
            raise InvalidBuildCacheException(
                "Failed to find {}, was the build generated.".format(hashes_file)
            )
        with open(hashes_file) as file_handle:
            lines = filter(
                lambda line: "{:x}".format(hash_value) in line, file_handle.readlines()
            )
        return list(lines)

    def get_build_cache(self) -> Path:
        """Generates build cache path for this build

        Generates the build path for this build. This will expect a valid build path to exist unless validate is
        specified as false. A valid build cache has been created from the generate step, and thus when using this call
        as part of the generate step, validate should be set to false.

        Returns:
            Path to a build cache directory

        """
        return self.deployment / Build.CMAKE_DEFAULT_BUILD_NAME.format(platform=self.platform,
                                                                       suffix=self.build_type.get_suffix())

    def get_build_info(self, context:Path) -> dict:
        """ Constructs an informational packet about this build

        Constructs a packet that allows for users to get meta-build information. This includes: location of build, file
        and other constructs, available make targets, and other items.

        Args:
            context: contextual path to list various information about the build

        Returns:

        """
        valid_cmake_targets = self.cmake.get_available_targets(str(self.build_dir), context)
        local_targets = [target for target in BUILD_TARGETS if target.cmake_target in valid_cmake_targets and isinstance(target, LocalTarget)]
        global_targets = [target for target in BUILD_TARGETS if isinstance(target, GlobalTarget)]

        relative_path = self.cmake.get_project_relative_path(str(context), self.build_dir)

        for possible in ["F-Prime", "."]:
            auto_location = self.build_dir / possible / relative_path
            if auto_location.exists():
                break
        else:
            auto_location = None
        return {"local_targets": local_targets, "global_targets": global_targets, "auto_location": auto_location}

    def execute(self, target:Target, context:Path, make_args:dict):
        """ Execute a build target

        Executes a target within the build system. This will execute the target by calling into the make system. Context
        is supplied such that the system can match local targets to the global target list.

        Args:
            target: target to run
            context: context path for local targets
            make_args: make system arguments directly supplied
        """
        self.cmake.execute_known_target(target.cmake_target, self.build_dir, context.absolute(), make_args=make_args,
                                        top_target=isinstance(target, GlobalTarget))

    def generate(self, cmake_args):
        try:
            cmake_args.update({"CMAKE_BUILD_TYPE": self.build_type.get_cmake_build_type()})
            self.cmake.generate_build(self.deployment, self.build_dir, cmake_args)
        except CMakeException:
            self.purge()


    def purge(self):
        self.cmake.purge(self.build_dir)

    @staticmethod
    def find_nearest_deployment(path: Path) -> Path:
        """ Recurse up the directory stack looking for a valid deployment

        Recurse up the directory tree from the given path, looking for a deployment definition directory. This means it
        defines a CMakeLists.txt with a project call. This finds where the automatic build directories are allowed to
        exist.

        Notes:
            This replaced the former build directory recursive detector as that can "slip" past a deployment should the
            build directory not be generated yet.

        Returns;
            Path to the nearest deployment directory searching up the directory tree

        Raises;
            UnableToDetectDeploymentException: was unable to detect a deployment directory
        """
        list_file = path / "CMakeLists.txt"
        if path == Path.anchor:
            raise UnableToDetectDeploymentException()
        elif list_file.exists():
            with open(list_file) as file_handle:
                text = file_handle.read()
            if Build.VALID_CMAKE_LIST.search(text):
                return path
        return Build.find_nearest_deployment(path.parent)

    def __setup_default(self, platform: str = None, build_dir: Path = None):
        """ Sets up default build

        Sets this build up before determining if it is a pre-generated, or post-generated build.

        build_dir is used to specify an exact build directory to use as part of this step. This allows directories to be
        specified by the caller, but is typically not used.

        Args:
            platform:   name of platform to build against. None will use default from settings.ini or without this
                        setting, "native". Defaults to None.
            build_dir:  explicitly sets the build path to allow for user override of default
        """
        assert self.settings is None, "Already setup it is invalid to re-setup"
        assert self.platform is None, "Already setup it is invalid to re-setup"
        assert self.build_dir is None, "Already setup it is invalid to re-setup"

        self.settings = IniSettings.load(self.deployment / "settings.ini")
        self.platform = platform if platform is not None else self.settings.get("default_toolchain", "native")
        self.build_dir = build_dir if build_dir is not None else self.get_build_cache()


class InvalidBuildCacheException(FprimeException):
    """ An exception indicating a build cache """
    pass


class UnableToDetectDeploymentException(FprimeException):
    """ An exception indicating a build cache """
    pass


class NoValidBuildTypeException(FprimeException):
    """ An build type matching the user request could not be found """
    pass


class NoSuchTargetExcetion(FprimeException):
    """ Could not find a matching build target """
    pass


""" Defined set of build targets available to the system"""
BUILD_TARGETS = [
    # Various "build" target
    LocalTarget("build", "Build components, ports, and deployments", cmake=""),
    GlobalTarget("build", "Build the top-level delpoyment targets", flags={"deployment"}),
    GlobalTarget("build", "Build all deployment targets", flags={"all"}, cmake="all"),
    LocalTarget("build", "Build unit tests", build_types=[BuildType.BUILD_TESTING],
                flags={"ut"}, cmake="ut_exe"),
    GlobalTarget("build", "Build deployment unit tests", build_types=[BuildType.BUILD_TESTING],
                 flags={"deployment", "ut"}, cmake="ut_exe"),
    # Implementation targets
    LocalTarget("impl", "Generate implementation template files"),
    LocalTarget("impl", "Generate unit test files", flags={"ut"}, cmake="testimpl"),
    # Check targets and unittest targets
    LocalTarget("check", "Run unit tests", build_types=[BuildType.BUILD_TESTING]),
    LocalTarget("check", "Run unit tests with memory checking",
                build_types=[BuildType.BUILD_TESTING], flags={"leak"}, cmake="check_leak"),
    GlobalTarget("check", "Run all deployment unit tests", build_types=[BuildType.BUILD_TESTING], flags={"all"}),
    GlobalTarget("check", "Run all deployment unit tests with memory checking",
                 build_types=[BuildType.BUILD_TESTING], flags={"all", "leak"}, cmake="check_leak"),
    LocalTarget("coverage", "Generate unit test coverage reports",
                build_types=[BuildType.BUILD_TESTING]),
    # Installation target
    GlobalTarget("install", "Install the current deployment build artifacts", build_types=[BuildType.BUILD_NORMAL])
]
