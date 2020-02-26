"""
fprime.fbuild.cmake:

File to contain basic wrappers for the CMake. This will enable basic CMake commands in order to detect the properties of
the build. This should not be imported in-person, but rather should be included by the build package. This can be the
receiver of these delegated functions.

@author mstarch
"""
import six
import io
import os
import re
import pty
import sys
import copy
import time
import shutil
import tempfile
import subprocess
import itertools
import functools
import collections
import selectors

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
        try:
            self._run_cmake(["--help"], print_output=False)
        except Exception as exc:
            raise CMakeExecutionException("CMake executable 'cmake' not found", str(exc), printed=False)

    def set_verbose(self, verbose):
        """ Sets verbosity """
        self.verbose = verbose

    def execute_known_target(self, target, build_dir, path, cmake_args=None, make_args=None, top_target=False):
        """
        Executes a known target for a given build_dir. Path will default to a known path.
        :param build_dir: build_dir to use to run this.
        :param target: target to execute at the path, using above build_dir
        :param path: path to run target against. (default) current working directory
        :param cmake_args: cmake args input
        :param top_target: top-level target. Do not append path name
        :return: return code from CMake
        """
        cmake_args = {} if cmake_args is None else cmake_args
        make_args = {} if make_args is None else make_args
        fleshed_args = list(map(lambda key: ("{}={}" if key.startswith("--") else "-D{}={}")
                           .format(key, cmake_args[key]), cmake_args.keys()))
        fleshed_args += ["--"] + list(map(lambda key: "{}={}".format(key, make_args[key]), make_args.keys()))
        # Get module name from the relative path to include root
        include_root = self.get_include_info(path, build_dir)[1]
        module = os.path.relpath(path,  include_root).replace(".", "").replace(os.sep, "_")
        cmake_target = module if target == "" else \
            ("{}_{}".format(module, target).lstrip("_") if not top_target else target)
        run_args = ["--build", build_dir]
        environment = {}
        if self.verbose:
            environment["VERBOSE"] = "1"
        run_args.extend(["--target", cmake_target])
        try:
            return self._run_cmake(run_args + fleshed_args, write_override=True, environment=environment)
        except CMakeExecutionException as exc:
            no_target = functools.reduce(lambda cur, line: cur or "No rule to make target" in line,
                                         exc.get_errors(), False)
            if not no_target:
                raise
            print("[CMAKE] CMake failed to detect target, attempting CMake cache refresh and retry")
            self._cmake_referesh_cache(build_dir)
            return self._run_cmake(run_args + fleshed_args, write_override=True, environment=environment)

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
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)
        # We will CD for build, so this path must become absolute
        source_dir = os.path.abspath(source_dir)
        args = {} if args is None else args
        fleshed_args = map(lambda key: ("{}={}" if key.startswith("--") else "-D{}={}")
                           .format(key, args[key]), args.keys())
        self._cmake_validate_source_dir(source_dir)
        self._run_cmake(["-S", source_dir] + list(fleshed_args), workdir=build_dir, print_output=not ignore_output,
                        write_override=True)

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
        stdout, stderr = self._run_cmake(["-LA"], workdir=build_dir, print_output=False)
        # Scan for lines in stdout that have non-None matches for the above regular expression
        valid_matches = filter(lambda item: item is not None, map(reg.match, stdout))
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

    def _cmake_referesh_cache(self, build_dir):
        """
        Runs the cmake  target required to refresh the cmake cache. This will allow for unknown targets to be searched
        for before the utility gives up and produces.
        :param build_dir: directory to build in
        """
        environment = {}
        run_args = ["--build", build_dir]
        if self.verbose:
            print("[CMAKE] Refreshing CMake build cache")
            environment["VERBOSE"] = "1"
        run_args.extend(["--target", "rebuild_cache"])
        self._run_cmake(run_args, write_override=True, environment=environment)

    def _run_cmake(self, arguments, workdir=None, print_output=True, write_override=False, environment={}):
        """
        Will run the cmake system supplying the given arguments. Assumes that the CMake executable is somewhere on the
        path in order for this to run.
        :param arguments: arguments to supply to CMake.
        :param workdir: work directory to run in
        :param print: print to the screen. Default: True
        :param write_override: allows for non-read-only commands
        :param environment: environment to write into
        :return: (stdout, stderr)
        Note: !!! this function has potential File System side-effects !!!
        """
        cm_environ = copy.copy(os.environ)
        cm_environ.update(environment)
        cargs = ["cmake"]
        if not write_override:
            cargs.append("-N")
        cargs.extend(arguments)
        if self.verbose:
            print("[CMAKE] '{}'".format(" ".join(cargs)))
        # In order to get proper console highlighting while still getting access to the output, we need to create a
        # pseudo-terminal. This will allow the proc to write to one side, and our select below to read from the other
        # side. Most importantly, pseudo-terminal usage will trick CMake into highlighting for us.
        pty_err_r, pty_err_w = pty.openpty()
        pty_out_r, pty_out_w = pty.openpty()
        proc = subprocess.Popen(cargs, stdout=pty_out_w, stderr=pty_err_w, cwd=workdir, env=cm_environ, close_fds=True)
        # Close our side of the writing pipes. Subproc will also close when it it finished.
        os.close(pty_out_w)
        os.close(pty_err_w)
        ret, stdout, stderr = self._communicate(proc, io.open(pty_out_r, mode="rb"), io.open(pty_err_r, mode="rb"),
                                                print_output)
        # Raising an exception when the return code is non-zero allows us to handle the exception internally if it is
        # needed. Thus we do not just exit.
        if ret != 0:
            raise CMakeExecutionException("CMake erred with return code {}".format(proc.returncode),
                                          stderr, print_output)
        return stdout, stderr

    @staticmethod
    def _communicate(proc, stdout, stderr, print_output=True):
        """
        Communicates with a process while assuring that output is captured and optionally printed. This will buffer
        lines for the standard out file handle when not none, and will always buffer standard error so that it can be
        provided when needed. This effectively replaces the .communicate method of the proc itself, while still
        preventing deadlocks.  The output is returned for each stream as a list of lines.
        :param proc: Popen process constructed with the above pairs to the submitted file handles
        :param stdout: standard output file handle. Paired with the FH provided to the Popen stdout argument
        :param stderr: standard error file handle. Paired with the FH provided to the Popen stderr argument
        :param print_output: print output to the screen. If False, output is masked. Default: True, print to screen.
        :return return code, stdout as list of lines, stderr as list of lines
        """
        stdouts = []
        stderrs = []
        # Selection will allow us to read from stdout and stderr whenever either is available. This will allow the
        # program to capture both, while still printing as soon as is possible. This will keep the user informed, not
        # merge streams, and capture output.
        #
        # Selection blocks on the read of "either" file descriptor, and then passes off the execution to the below code
        # with a key that represents which descriptor was the one available to read without blocking.
        selector = selectors.DefaultSelector()
        selector.register(stdout, selectors.EVENT_READ, data=(stdouts, sys.stdout))
        selector.register(stderr, selectors.EVENT_READ, data=(stderrs, sys.stderr))
        while not stdout.closed or not stderr.closed:
            # This line *BLOCKS* until on of the above registered handles is available to read. Then a set of events is
            # returned signaling that a given object is available for IO.
            events = selector.select()
            for key, _ in events:
                appendable, stream = key.data
                try:
                    line = key.fileobj.readline().decode().replace("\r\n", "\n")
                # Some systems (like running inside Docker) raise an io error instead of returning "" when the device
                # is ended. Not sure why this is, but the effect is the same, on IOError assume end-of-input
                except IOError as ioe:
                    line = ""
                appendable.append(line)
                # Streams are EOF when the line returned is empty. Once this occurs, we are responsible for closing the
                # stream and thus closing the select loop. Empty strings need not be printed.
                if line == "":
                    key.fileobj.close()
                    continue
                # Forwards output to screen.  Assuming a PTY is used, then coloring highlights should be automatically
                # included for output. Raw streams are used to avoid print quirks
                elif print_output:
                    stream.write(line)
                    stream.flush()
        # Spin waiting for the .poll() method to return a non-None result ensuring that the process has finished.
        while proc.poll() is None:
            time.sleep(0.0001)
        return proc.poll(), stdouts, stderrs


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
            .__init__("{} is not a CMake build directory. Please setup using 'fprime-util generate"
                      .format(build_dir, build_dir))


class CMakeExecutionException(CMakeException):
    """ Pass up a CMake Error as an Exception """
    def __init__(self, message, stderr, printed):
        """ The error data should be stored """
        super(CMakeExecutionException, self).__init__(message)
        self.stderr = stderr
        self.need = not printed

    def get_errors(self):
        """
        Returns the error stream data to the caller
        :return: stderr of CMake as supplied into this Exception
        """
        return self.stderr

    def need_print(self):
        """
        Returns if the errors need to be printed
        :return: need print
        """
        return self.need

class CMakeNoSuchTargetException(CMakeException):
    """ The target does not exist. """
    def __init__(self, build_dir, target):
        """  Better messaging for this exception """
        super(CMakeNoSuchTargetException, self).__init__("{} does not support target {}".format(build_dir, target))
