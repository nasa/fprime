# F´ Build Helper Utility

The `fprime-util` is a program designed to help developers run the standard development process by automating many of the
standard functions built into the build system. It is intended to remove some of the inefficiencies using CMake while keeping
the advantages of that build system.

## Basic Usage
```
usage: fprime-util [-h]
                   {generate,purge,build,impl,impl-ut,build-ut,check,check-all,install,build-all,hash-to-file}
                   ...

F prime helper application.

optional arguments:
  -h, --help            show this help message and exit

subcommands:
  F prime utility command line. Please run one of the commands. For help,
  run a command with the --help flag.

  {generate,purge,build,impl,impl-ut,build-ut,check,check-all,install,build-all,hash-to-file}
```

The `fprime-util` helper is driven by a series of subcommands listed above. Each perform one aspect of the development process
and are breifly described below.  The following sections will go into each command's esage in more detail.

1. `generate`: generates build caches, one for development, and one with unit test flags enabled. This is the required first
step to using the F´ build system. This should be executed once int the deployment directory before developing.  It can be run
again after the user runs `purge`.
2. `purge`: removes the build caches created with generate. This should be used when the build system has not properly detected
changes and need to be redone from the beginning.
3. `build`: builds the current directory. If in a component directory, it will compile the component library. If in a
deployment directory, it will build the binary.
4. `build-ut`: builds any unit tests in the current directory.
5. `build-all`: builds all units in the project. Usefull to build tools, or other secondary products inside an F prime project.
6. `impl`: generates implementation stubs for the developer to fill in.
7. `impl-ut`: generates implementation stubs for unit testing.
8. `check`: runs any unit tests in the current directory.
9. `check-all`: runs all unit tests in the project.
10. `install`: installs all executables built to `bin` and all libraries to `lib` directories under the current project
11. `hash-to-file`: convert an assert file id (hash) to the source file in the build.

## Standard Arguments

Most commands above take a standard set of arguments. These arguments can allow more advanced usage of the tool, and are 
described here.

```
...command help...
positional arguments:
  platform              F prime build platform (e.g. Linux, Darwin). Default
                        specified in CMakeLists.txt.

optional arguments:
  -h, --help            show this help message and exit
  -b BUILD_DIR, --build-dir BUILD_DIR
                        F prime build directory to generate, or existing build
                        to operate on.
  -p PATH, --path PATH  F prime directory to operate on. Default: cwd,
                        /Users/mstarch/code/fprime.
  -j JOBS, --jobs JOBS  F prime parallel job count. Default: 1.
  -v, --verbose         Turn on verbose output.
```

First of all, all commands may specify a "platform" as a positional argument.  This platform is how users may cross-compile the
F´ code. `generate` will create a separate build cache, and thus generate must proceed builds of that platform. the platform 
will take the name of any standard F´ toolchain or the path to any CMake Toolchain and platform.cmake pair to use as a cross-
compile. See CMake documentation for specifying these files.

`-b BUILD_DIR --build_dir BUILD_DIR` will sepcify the full directory for an F´ build cache generated with the `generate` 
command. This allows users to select a specific build cache, especially if it cannot be automatically detected.  If not set, 
the buil cache matching the platform specified will be chosen in the nearest parent directory of the component.

`-p PATH --path, PATH` path to operate on. Defaults to the current directory.  This allows users to operate on non-CWD pathes.


`-j JOBS, --jobs JOBS` allows for parallel execution. Set the number of jobs to use when building and executing commands.  Not
used for non-building commands.

`-v, --verbose` forces verbose output.  If an error occurs, this will provide more information.  Rerun with this flag 
specified.

