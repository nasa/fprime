"""
fprime.fbuild.cmake:

File to contain basic wrappers for the CMake. This will enable basic CMake commands in order to detect the properties of
the build. This should not be imported in-person, but rather should be included by the build package. This can be the
receiver of these delegated functions.

@author mstarch
"""
import six
import os
import re
import sys
import shutil
import tempfile
import subprocess
import itertools
import functools
import collections

# Get a cache directory for building CMakeList file, if need and remove at exit
import atexit
import fprime.fbuild

class CMakeBuildCache(object):
    """
    Builds CMake deployment for the purposes of inspecting that build. This exists because generating a build on every
    call take a long time. This cache will hold the results to prevent recalculation.
    """
    def __init__(self):
        """ Sets up the known project to None """
        self.project = None
        self.tempdir = None

    def get_cmake_temp_build(self, proj_dir, verbose=False):
        """ Gets a CMake build directory for the specified proj_dir """
        if self.project is not None and proj_dir is not None and self.project != proj_dir:
            raise CMakeException("Already tracking project {}".format(self.project))
        # No tempdir, prepare a build
        if self.tempdir is None:
            if proj_dir is None:
                raise ValueError("No build cache available, and no project_dir specified")
            # Create a temp directory and register its deletion at the end of the program run
            self.tempdir = tempfile.mkdtemp()
            atexit.register(lambda: shutil.rmtree(self.tempdir, ignore_errors=True))
            # Turn that directory into a CMake build
            fprime.fbuild.builder().generate_build(proj_dir, self.tempdir, ignore_output=not verbose)
        self.project = proj_dir
        return self.tempdir


class CMakeHandler(object):
    """
    CMake handler interacts with an F prime CMake-based system. This will help us interact with CMake in refined ways.
    """
    CMAKE_DEFAULT_BUILD_NAME = "build-fprime-automatic-{}"
    CMAKE_LOCATION_FIELDS = ["FPRIME_PROJECT_ROOT", "FPRIME_LIBRARIES", "FPRIME_FRAMEWORK_PATH"]

    def __init__(self):
        """
        Instantiate a basic CMake handler.
        """
        self.build_cache = CMakeBuildCache()
        self.verbose = False

    def set_verbose(self, verbose):
        """ Sets verbosity """
        self.verbose = verbose

    def execute_known_target(self, target, build_dir, path, cmake_args=None):
        """
        Executes a known target for a given build_dir. Path will default to a known path.
        :param build_dir: build_dir to use to run this.
        :param target: target to execute at the path, using above build_dir
        :param path: path to run target against. (default) current working directory
        :param cmake_args: cmake args input
        :return: return code from CMake
        """
        cmake_args = {} if cmake_args is None else cmake_args
        fleshed_args = list(map(lambda key: ("{}={}" if key.startswith("--") else "-D{}={}")
                           .format(key, cmake_args[key]), cmake_args.keys()))
        # Get module name from the relative path to include root
        include_root = self.get_include_info(path, build_dir)[1]
        module = os.path.relpath(path,  include_root).replace(".", "").replace(os.sep, "_")
        cmake_target = module if target == "" else "{}_{}".format(module, target).lstrip("_")
        run_args = ["--build", build_dir]
        if self.verbose:
            run_args.append("--verbose")
        run_args.extend(["--target", cmake_target])
        return self._run_cmake(run_args + fleshed_args, write_override=True)

    def find_hashed_file(self, build_dir, hash):
        """
        Find a file from a hash
        :param build_dir: build directory to search
        :param hash: hash number
        :return: filename
        """
        hashes_file = os.path.join(build_dir, "hashes.txt")
        if not os.path.exists(hashes_file):
            raise CMakeException("Failed to find {}, was the build generated.".format(hashes_file))
        with open(hashes_file, "r") as file_handle:
            lines = filter(lambda line: "{:x}".format(hash) in line, file_handle.readlines())
        return list(lines)

    def find_nearest_standard_build(self, platform, path):
        """
        Recurse up the directory tree from the given path, looking for a directory that matches the standard name. This
        will return that path, or error if one cannot be found.
        :return: path of nearest standard build directory
        :throws CMakeInvalidBuildException, a build was found but not setup, CMakeOrphanException, no build was found
        """
        path = os.path.abspath(path)
        # Get current directory to be checked, by removing file if path is not already a directory
        current = path if os.path.isdir(path) else os.path.dirname(path)
        # Look for a potential build that is valid
        potential_build = os.path.join(current, CMakeHandler.CMAKE_DEFAULT_BUILD_NAME.format(platform))
        if os.path.exists(potential_build):
            self._cmake_validate_build_dir(potential_build)
            return potential_build
        # Check for root, and throw error if it is already a root
        new_dir = os.path.dirname(current)
        if new_dir == path:
            raise CMakeException("{} not in ancestor tree"
                                 .format(CMakeHandler.CMAKE_DEFAULT_BUILD_NAME.format(platform)))
        return self.find_nearest_standard_build(platform, new_dir)

    def get_include_locations(self, cmake_dir):
        """
        Gets the locations that can be used as the root of an include tree. Common directories are placed in these
        include locations. These include standard builds, configs, etc.
        :param cmake_dir: directory of a CMake build, or directory containing a CMake project
        :return: [] # List of include locations. Order: project, lib, lib, ..., F prime core
        !!! Note: supplying a project dir as cmake_dir will setup a build-cache, and thus take much time. !!!
        """
        # Reading config fields. If the cmake_dir is a project dir, a build cache may be setup.
        # !! Note: using a project dir will cause file-system side effects, and incur a one-time cost to setup cache !!
        config_fields = self.get_fprime_configuration(CMakeHandler.CMAKE_LOCATION_FIELDS, cmake_dir)
        non_null = filter(lambda item: item is not None, config_fields)
        # Read cache fields for each possible directory the build_dir, and the new tempdir
        locations = itertools.chain(*map(lambda value: value.split(";"), non_null))
        mapped = map(os.path.abspath, locations)
        # Removes duplicates, by creating an ordered dictionary, and then asking for its keys
        return list(collections.OrderedDict.fromkeys(mapped).keys())

    def get_include_info(self, path, cmake_dir):
        """
        Calculates the include root of the given path. The include root is defined as the following based on the other
        two values supplied. First, the following two path values are established:
           - Location of the project's root. This is defined in the project_dir's CMakeList.txt, or in the CMake Cache.
           - Location of the project's F prime checkout. This is defined in the same places.
        From there, the include root of the supplied path is whichever of those two paths is your parent. In cases where
        both are parents, it will take the outer-most parent
        :param path: path to calculate looking for include-root
        :param cmake_dir: directory of a CMake build, or directory containing a CMake project
        :return: (relative include path, include root for the given path)
        !!! Note: supplying a project dir as cmake_dir will setup a build-cache, and thus take much time. !!!
        """
        path = os.path.abspath(path) if path is not None else os.path.abspath(os.getcwd())
        possible_parents = self.get_include_locations(cmake_dir)
        # Check there is some possible parent
        if not possible_parents:
            raise CMakeProjectException(cmake_dir, "Does not define any of the cache fields: {}"
                                        .format(",".join(CMakeHandler.CMAKE_LOCATION_FIELDS)))
        full_parents = map(os.path.abspath, possible_parents)
        # Parents *are* the common prefix for their children
        parents = filter(lambda parent: os.path.commonprefix([parent, path]) == parent, full_parents)

        def parent_reducer(accum, item):
            """
            Reduces a list of parents, and the given path, to a single path who is the closes parent to the existing
            path while stilling being a parent.
            :param accum: latest best parent, or None if no valid parent found
            :param item: current parent being evaluated
            :return: next latest best parent
            """
            # None items cannot weigh-in
            if item is None:
                return accum
            # Calculate common path
            prefix = os.path.commonprefix([item, path])
            # Return the previous parent, if new prefix is not the directory itself, or the old one was closer
            return accum if prefix != item or (accum is not None and len(accum) > len(item)) else item
        nearest_parent = functools.reduce(parent_reducer, parents, None)
        # Check that a parent is the true parent
        if nearest_parent is None:
            raise CMakeOrphanException(path)
        return os.path.relpath(path, nearest_parent), nearest_parent

    def get_fprime_configuration(self, fields, cmake_dir=None):
        """
        Gets fprime configuration for the given field(s). This will return a list of fields for the set of input fields.
        The user may supply a string for a single value returned as list of one, or a list of fields for list of values.
        :param fields: name of field, or list of names of fields
        :param cmake_dir: a cmake directory (project or build) to used. default: None, use existing temp cached build.
        :return: list of values, or Nones
        """
        if isinstance(fields, six.string_types):
            fields = [fields]
        # Setup the build_dir if it can be detected. Without a cache or specified value, we can crash
        build_dir = self._build_directory_from_cmake_dir(cmake_dir)
        return self._read_values_from_cache(fields, build_dir=build_dir)

    def _build_directory_from_cmake_dir(self, cmake_dir):
        """
        If the supplied directory is a valid CMake build directory, then this will be returned. Otherwise, the file
        should be a valid CMake project directory containing a CMakeLists.txt with a project call. This will then
        generate a temporary directory to be used as a build.
        :return: working build directory
        """
        try:
            self._cmake_validate_build_dir(cmake_dir)
            return cmake_dir
        except (CMakeInvalidBuildException, TypeError):
            return self.build_cache.get_cmake_temp_build(cmake_dir, self.verbose)

    def generate_build(self, source_dir, build_dir, args=None, ignore_output=False):
        """
        Generate a build directory for purposes of the build.
        :param source_dir: source directory to generate from
        :param build_dir: build directory to generate to
        :param args: arguments to hand to CMake.
        :param ignore_output: do not print the output where the user can see it
        """
        args = {} if args is None else args
        fleshed_args = map(lambda key: ("{}={}" if key.startswith("--") else "-D{}={}")
                           .format(key, args[key]), args.keys())
        self._cmake_validate_source_dir(source_dir)
        self._run_cmake(["-S", source_dir, "-B", build_dir] + list(fleshed_args),
                                capture=ignore_output, write_override=True)

    def _read_values_from_cache(self, keys, build_dir):
        """
        Reads set values from cache into an output tuple.
        :param keys: keys to read in iterable
        :param build_dir: build directory containing cache file
        :return: a tuple of keys, None if not part of cache
        """
        cache = self._read_cache(build_dir)
        # Reads cache values suppressing KeyError, {}.get(x, default=None)
        miner = lambda x: cache.get(x, None)
        return tuple(map(miner, keys))

    def _read_cache(self, build_dir):
        """
        Reads the cache from the associated build_dir. This will return a dictionary of cache variable name to
        its value. This will not update internal state.
        :param build_dir: build directory to harvest for cache variables
        :return: {<cmake cache variable>: <cmake cache value>}
        """
        reg = re.compile("([^:]+):[^=]*=(.*)")
        # Check that the build_dir is properly setup
        self._cmake_validate_build_dir(build_dir)
        stdout, stderr = self._run_cmake(["-B", build_dir, "-LA"], capture=True)
        # Scan for lines in stdout that have non-None matches for the above regular expression
        valid_matches = filter(lambda item: item is not None, map(reg.match, stdout.split("\n")))
        # Return the dictionary composed from the match groups
        return dict(map(lambda match: (match.group(1), match.group(2)), valid_matches))

    @staticmethod
    def _cmake_validate_source_dir(source_dir):
        """
        Raises an exception if the source dir is not a valid CMake source directory. This means a CMakeLists.txt exists
        and defines within it a project call.
        :param source_dir: source directory to validate
        """
        cmake_file = os.path.join(source_dir, "CMakeLists.txt")
        if not os.path.isfile(cmake_file):
            raise CMakeProjectException(source_dir, "No CMakeLists.txt is defined")
        # Test the cmake_file for project(
        with open(cmake_file, "r") as file_handle:
            project_lines = list(filter(lambda line: "project(" in line, file_handle.readlines()))
            if not project_lines:
                raise CMakeProjectException(source_dir, "No 'project()' calls in {}".format(cmake_file))

    @staticmethod
    def _cmake_validate_build_dir(build_dir):
        """
        Raises an exception if the build dir is not a valid CMake build directory
        :param build_dir: build_dir to validate
        """
        cache_file = os.path.join(build_dir, "CMakeCache.txt")
        if not os.path.isfile(cache_file):
            raise CMakeInvalidBuildException(build_dir)

    def _run_cmake(self, arguments, capture=False, write_override=False):
        """
        Will run the cmake system supplying the given arguments. Assumes that the CMake executable is somewhere on the
        path in order for this to run.
        :param arguments: arguments to supply to CMake.
        :param write_override: allows for non-read-only commands
        :return: (stdout, stderr)
        Note: !!! this function has potential File System side-effects !!!
        """
        # Keep these steps atomic
        cargs = ["cmake"]
        if not write_override:
            cargs.append("-N")
        cargs.extend(arguments)
        # Verbose means print calls
        if self.verbose:
            print("[CMAKE] '{}'".format(" ".join(cargs)))
        proc = subprocess.Popen(cargs, stdout=subprocess.PIPE if capture else None,
                                stderr=subprocess.PIPE if capture else None)
        stdout, stderr = proc.communicate()
        # Check for Python 3, and decode if possible
        if capture and sys.version_info[0] >= 3:
            stdout = stdout.decode()
            stderr = stderr.decode()
        # Raise for errors
        if proc.returncode != 0:
            raise CMakeExecutionException("CMake erred with return code {}".format(proc.returncode), stderr)
        return stdout, stderr


class CMakeException(Exception):
    """ Error occurred within this CMake package """
    pass


class CMakeInconsistencyException(CMakeException):
    """ Project CMakeLists.txt is inconsistent with build dir """
    def __init__(self, project_cmake, build_dir):
        """ Force an appropriate message """
        super(CMakeInconsistencyException, self).__init__("{} is inconsistent with build {}. Regenerate the build"
                                                          .format(project_cmake, build_dir))


class CMakeOrphanException(CMakeException):
    """ File is not managed by CMake project """
    def __init__(self, file_dir):
        """ Force an appropriate message """
        super(CMakeOrphanException, self).__init__("{} is outside the F prime project".format(file_dir))


class CMakeProjectException(CMakeException):
    """ Invalid project directory """
    def __init__(self, project_dir, error):
        """ Force an appropriate message """
        super(CMakeProjectException, self)\
            .__init__("{} is an invalid F prime deployment. {}".format(project_dir, error))


class CMakeInvalidBuildException(CMakeException):
    """ The supplied build directory was not setup as a CMake value """
    def __init__(self, build_dir):
        """ Force an appropriate message """
        super(CMakeInvalidBuildException, self)\
            .__init__("{} is not a CMake build directory. Please setup using 'cmake -B {} <path to deployment>'"
                      .format(build_dir, build_dir))


class CMakeExecutionException(CMakeException):
    """ Pass up a CMake Error as an Exception """
    def __init__(self, message, stderr):
        """ The error data should be stored """
        super(CMakeExecutionException, self).__init__(message)
        self.stderr = stderr

    def get_errors(self):
        """
        Returns the error stream data to the caller
        :return: stderr of CMake as supplied into this Exception
        """
        return self.stderr

class CMakeNoSuchTargetException(CMakeException):
    """ The target does not exist. """
    def __init__(self, build_dir, target):
        """  Better messaging for this exception """
        super(CMakeNoSuchTargetException, self).__init__("{} does not support target {}".format(build_dir, target))
