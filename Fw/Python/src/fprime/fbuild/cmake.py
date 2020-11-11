"""
fprime.fbuild.cmake:

File to contain basic wrappers for the CMake. This will enable basic CMake commands in order to detect the properties of
the build. This should not be imported in-person, but rather should be included by the build package. This can be the
receiver of these delegated functions.

@author mstarch
"""
# Get a cache directory for building CMakeList file, if need and remove at exit
import collections
import copy
import functools
import itertools
import os
import pty
import re
import selectors
import shutil
import subprocess
import sys
import time

from fprime.common.error import FprimeException


class CMakeHandler:
    """
    CMake handler interacts with an F prime CMake-based system. This will help us interact with CMake in refined ways.
    """

    CMAKE_LOCATION_FIELDS = [
        "FPRIME_PROJECT_ROOT",
        "FPRIME_LIBRARY_LOCATIONS",
        "FPRIME_FRAMEWORK_PATH",
    ]

    def __init__(self):
        """ Instantiate a basic CMake handler """
        self.settings = {}
        # self.build_cache = CMakeBuildCache()
        self.verbose = False
        try:
            self._run_cmake(["--help"], print_output=False)
        except Exception as exc:
            raise CMakeExecutionException(
                "CMake executable 'cmake' not found", str(exc), printed=False
            ) from exc

    def set_verbose(self, verbose):
        """ Sets verbosity """
        self.verbose = verbose

    def execute_known_target(
        self, target, build_dir, path, cmake_args=None, make_args=None, top_target=False, environment=None
    ):
        """
        Executes a known target for a given build_dir. Path will default to a known path.

        :param build_dir: build_dir to use to run this.
        :param target: target to execute at the path, using above build_dir
        :param path: path to run target against. (default) current working directory
        :param cmake_args: cmake args input
        :param top_target: top-level target. Do not append path name
        :param environment: environment to setup when executing CMake
        :return: return code from CMake
        """
        assert build_dir is not None, "Invalid build dir supplied"
        build_dir = str(build_dir)
        cmake_args = {} if cmake_args is None else cmake_args
        make_args = {} if make_args is None else make_args
        fleshed_args = list(
            map(
                lambda key: ("{}={}" if key.startswith("--") else "-D{}={}").format(
                    key, cmake_args[key]
                ),
                cmake_args.keys(),
            )
        )
        fleshed_args += ["--"] + list(
            map(lambda key: "{}={}".format(key, make_args[key]), make_args.keys())
        )
        module = self.get_cmake_module(path, build_dir)
        cmake_target = (
            module
            if target == ""
            else (
                "{}_{}".format(module, target).lstrip("_") if not top_target else target
            )
        )
        run_args = ["--build", build_dir]
        environment = {} if environment is None else copy.copy(environment)
        if self.verbose:
            environment["VERBOSE"] = "1"
        run_args.extend(["--target", cmake_target])
        try:
            return self._run_cmake(
                run_args + fleshed_args, write_override=True, environment=environment
            )
        except CMakeExecutionException as exc:
            no_target = functools.reduce(
                lambda cur, line: cur or "No rule to make target" in line,
                exc.get_errors(),
                False,
            )
            if not no_target:
                raise
            print(
                "[CMAKE] CMake failed to detect target, attempting CMake cache refresh and retry"
            )
            self._cmake_referesh_cache(build_dir)
            return self._run_cmake(
                run_args + fleshed_args, write_override=True, environment=environment
            )

    def get_include_locations(self, cmake_dir):
        """
        Gets the locations that can be used as the root of an include tree. Common directories are placed in these
        include locations. These include standard builds, configs, etc.

        :param cmake_dir: directory of a CMake build, or directory containing a CMake project
        :return: []  List of include locations. Order: project, lib, lib, ..., F prime core
        """
        # Reading config fields. If the cmake_dir is a project dir, a build cache may be setup.
        # !! Note: using a project dir will cause file-system side effects, and incur a one-time cost to setup cache !!
        config_fields = self.get_fprime_configuration(
            CMakeHandler.CMAKE_LOCATION_FIELDS, cmake_dir
        )
        non_null = filter(lambda item: item is not None and item != "", config_fields)
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
        """
        path = (
            os.path.abspath(path) if path is not None else os.path.abspath(os.getcwd())
        ) + os.sep
        possible_parents = self.get_include_locations(cmake_dir)
        # Check there is some possible parent
        if not possible_parents:
            raise CMakeProjectException(
                cmake_dir,
                "Does not define any of the cache fields: {}".format(
                    ",".join(CMakeHandler.CMAKE_LOCATION_FIELDS)
                ),
            )
        # Common-prefix will fail in the case that two directories share a common prefix in their name
        # So force directory separator to force common prefix to only work on full directory names
        full_parents = map(lambda dir: os.path.abspath(dir) + os.sep, possible_parents)
        # Parents *are* the common prefix for their children
        parents = filter(
            lambda parent: os.path.commonprefix([parent, path]) == parent, full_parents
        )

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
            return (
                accum
                if prefix != item or (accum is not None and len(accum) > len(item))
                else item
            )

        parents = list(parents)
        nearest_parent = functools.reduce(parent_reducer, parents, None)
        # Check that a parent is the true parent
        if nearest_parent is None:
            raise CMakeOrphanException(path)
        return os.path.relpath(path, nearest_parent), nearest_parent.rstrip(os.sep)

    def get_fprime_configuration(self, fields, cmake_dir=None):
        """
        Gets fprime configuration for the given field(s). This will return a list of fields for the set of input fields.
        The user may supply a string for a single value returned as list of one, or a list of fields for list of values.

        :param fields: name of field, or list of names of fields
        :param cmake_dir: a cmake directory (project or build) to used. default: None, use existing temp cached build.
        :return: list of values, or Nones
        """
        if isinstance(fields, str):
            fields = [fields]
        # Setup the build_dir if it can be detected. Without a cache or specified value, we can crash
        self._cmake_validate_build_dir(cmake_dir)  # Validate the dir
        return self._read_values_from_cache(fields, build_dir=cmake_dir)

    def generate_build(self, source_dir, build_dir, args=None, ignore_output=False, environment=None):
        """Generate a build directory for purposes of the build.

        :param source_dir: source directory to generate from
        :param build_dir: build directory to generate to
        :param args: arguments to hand to CMake.
        :param ignore_output: do not print the output where the user can see it
        :param environment: environment to set when generating
        """

        if not os.path.exists(build_dir):
            os.makedirs(build_dir)
        # We will CD for build, so this path must become absolute
        source_dir = os.path.abspath(source_dir)
        args = {} if args is None else args
        if "FPRIME_LIBRARY_LOCATIONS" in args:
            args["FPRIME_LIBRARY_LOCATIONS"] = ";".join(
                args["FPRIME_LIBRARY_LOCATIONS"]
            )
        fleshed_args = map(
            lambda key: ("{}={}" if key.startswith("--") else "-D{}={}").format(
                key, args[key]
            ),
            args.keys(),
        )
        self._cmake_validate_source_dir(source_dir)
        self._run_cmake(
            ["-S", source_dir] + list(fleshed_args),
            workdir=build_dir,
            print_output=not ignore_output,
            write_override=True,
            environment=environment
        )

    def get_cmake_module(self, path, build_dir):
        """Gets the CMake module

        CMake modules are constructed from a path relative to some project root (fprime, deployment, or library roots).
        This relative path is then converted to use "_" instead of "/"

        Args:
            path: path to contextualize. May be None to use os.getcwd().
            build_dir: build directory to use

        Returns:
            CMake module name in format x_y_z
        """
        project_relative_path = self.get_project_relative_path(path, build_dir)
        module = project_relative_path.replace(
            ".", ""
        )  # Handles case where relative path is exactly "."
        module = module.replace(os.sep, "_")
        return module

    def get_project_relative_path(self, path, build_dir):
        """Gets the path relative to the cmake setup, or raises CMakeOrphanException

        Args:
            path: path to contextualize. May be None to use os.getcwd().
            build_dir: build directory to use

        Returns:
            path string that is relative to some root of the project. i.e. used for target suffix names
        """
        # Get module name from the relative path to include root
        include_root = self.get_include_info(path, build_dir)[1]
        return os.path.relpath(path, include_root)

    def get_available_targets(self, build_dir, path):
        """Gets a list of available CMake targets in the current directory

        Args:
            build_dir: build directory to use for detecting targets
            path: contextual path. None for "cwd"

        Note:
            This code might not work on non-GNU makefile variants of CMake as it depends on the "help" target

        Returns:
            list of CMake make targets
        """
        run_args = ["--build", build_dir, "--target", "help"]
        stdout, _ = self._run_cmake(run_args, write_override=True, print_output=False)
        prefix = self.get_cmake_module(path, build_dir)

        make_target_names = [
            line.replace("...", "").strip() for line in stdout if line.startswith("...")
        ]
        contextual_make_targets = [
            make.replace(prefix, "").strip("_")
            for make in make_target_names
            if make.startswith(prefix)
        ]
        return contextual_make_targets

    @staticmethod
    def purge(build_dir):
        """
        Reusable purge functionality, so the user may purge or the system may cleanup itself

        :param build_dir: build dir specified to purge
        """
        shutil.rmtree(build_dir, ignore_errors=True)

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
        with open(cmake_file) as file_handle:
            project_lines = list(
                filter(lambda line: "project(" in line, file_handle.readlines())
            )
            if not project_lines:
                raise CMakeProjectException(
                    source_dir, "No 'project()' calls in {}".format(cmake_file)
                )

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

    def _run_cmake(
        self,
        arguments,
        workdir=None,
        print_output=True,
        write_override=False,
        environment=None,
    ):
        """
        Will run the cmake system supplying the given arguments. Assumes that the CMake executable is somewhere on the
        path in order for this to run.

        :param arguments: arguments to supply to CMake.
        :param workdir: work directory to run in
        :param print_output: print_output to the screen. Default: True
        :param write_override: allows for non-read-only commands
        :param environment: environment to write into
        :return: (stdout, stderr)
        """
        if environment is None:
            environment = {}

        cm_environ = os.environ.copy()
        cm_environ.update(self.settings.get("environment", {}))
        cm_environ.update(environment)
        cargs = ["cmake"]
        if not write_override:
            cargs.append("-N")
        cargs.extend(arguments)
        if self.verbose:
            print("[CMAKE] '{}'".format(" ".join(cargs)))
            for key, val in cm_environ.items():
                print("[CMAKE]     {}={}".format(key, val))

        # In order to get proper console highlighting while still getting access to the output, we need to create a
        # pseudo-terminal. This will allow the proc to write to one side, and our select below to read from the other
        # side. Most importantly, pseudo-terminal usage will trick CMake into highlighting for us.
        pty_err_r, pty_err_w = pty.openpty()
        pty_out_r, pty_out_w = pty.openpty()
        proc = subprocess.Popen(
            cargs,
            stdout=pty_out_w,
            stderr=pty_err_w,
            cwd=workdir,
            env=cm_environ,
            close_fds=True,
        )
        # Close our side of the writing pipes. Subproc will also close when it it finished.
        os.close(pty_out_w)
        os.close(pty_err_w)
        ret, stdout, stderr = self._communicate(
            proc,
            open(pty_out_r, mode="rb"),
            open(pty_err_r, mode="rb"),
            print_output,
        )
        # Raising an exception when the return code is non-zero allows us to handle the exception internally if it is
        # needed. Thus we do not just exit.
        if ret != 0:
            raise CMakeExecutionException(
                "CMake erred with return code {}".format(proc.returncode),
                stderr,
                print_output,
            )
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
                except OSError:
                    line = ""
                appendable.append(line)
                # Streams are EOF when the line returned is empty. Once this occurs, we are responsible for closing the
                # stream and thus closing the select loop. Empty strings need not be printed.
                if line == "":
                    key.fileobj.close()
                    continue
                # Forwards output to screen.  Assuming a PTY is used, then coloring highlights should be automatically
                # included for output. Raw streams are used to avoid print quirks
                if print_output:
                    stream.write(line)
                    stream.flush()
        # Spin waiting for the .poll() method to return a non-None result ensuring that the process has finished.
        while proc.poll() is None:
            time.sleep(0.0001)
        return proc.poll(), stdouts, stderrs


class CMakeException(FprimeException):
    """ Error occurred within this CMake package """


class CMakeInconsistencyException(CMakeException):
    """ Project CMakeLists.txt is inconsistent with build dir """

    def __init__(self, project_cmake, build_dir):
        """ Force an appropriate message """
        super().__init__(
            "{} is inconsistent with build {}. Regenerate the build".format(
                project_cmake, build_dir
            )
        )


class CMakeOrphanException(CMakeException):
    """ File is not managed by CMake project """

    def __init__(self, file_dir):
        """ Force an appropriate message """
        super().__init__("{} is outside the F prime project".format(file_dir))


class CMakeProjectException(CMakeException):
    """ Invalid project directory """

    def __init__(self, project_dir, error):
        """ Force an appropriate message """
        super().__init__(
            "{} is an invalid F prime deployment. {}".format(project_dir, error)
        )


class CMakeInvalidBuildException(CMakeException):
    """ The supplied build directory was not setup as a CMake value """

    def __init__(self, build_dir):
        """ Force an appropriate message """
        super().__init__(
            "{} is not a CMake build directory. Please setup using 'fprime-util generate'".format(
                build_dir
            )
        )


class CMakeExecutionException(CMakeException):
    """ Pass up a CMake Error as an Exception """

    def __init__(self, message, stderr, printed):
        """ The error data should be stored """
        super().__init__(message)
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
        super().__init__("{} does not support target {}".format(build_dir, target))
