The `fprime-util` is a program designed to help developers run the standard development process by
automating many of the standard functions built into the build system. It is intended to remove some
of the inefficiencies using CMake while keeping the advantages of that build system.

## Basic Usage
```
usage: fprime-util [-h] {generate,purge,hash-to-file,info,build,impl,check,coverage,install} ...

F prime helper application.

optional arguments:
  -h, --help            show this help message and exit

subcommands:
  F prime utility command line. Please run one of the commands. For help, run a command with the --help flag.

  {generate,purge,hash-to-file,info,build,impl,check,coverage}
    generate            Generate a build cache directory
    purge               Purge build cache directories
    hash-to-file        Converts F prime build hash to filename.
    info                Gets fprime-util contextual information.
    build               Build components, ports, and deployments in the specified directory
    impl                Generate implementation template files in the specified directory
    check               Run unit tests in the specified directory
    coverage            Generate unit test coverage reports in the specified directory
    new                 Create new component or port
```

The `fprime-util` helper is driven by a series of subcommands listed above. Each performs one aspect
of the development process and they are briefly described below. The following sections will go into
each command's usage in more detail.

1. `generate`: generates build cache directories. It defaults to generating the build cache for
   release builds, but adding the `--ut` flag will generate a unit testing build cache. This is the
   required first step to using the F´ build system and must be run before building an F´ deployment
   or running unit tests. The `purge` command can be used to delete all existing build caches.
2. `purge`: removes the build caches created with generate. This should be used when the build
   system has not properly detected changes and needs to be redone from the beginning.
3. `hash-to-file`: If F´ assertions are set up to output file hashes instead of file paths (i.e. when
   `#define FW_ASSERT_LEVEL = FW_FILEID_ASSERT`), convert a given file hash to a full filepath.
4. `info`: Print information about the location of build caches and `fprime-util` commands available in
   the current directory.
5. `build`: builds the current directory. If in a component directory, it will compile the component
   library. If in a deployment directory, it will build the binary and the copy build artifacts into
   the installation directory.
6. `impl`: Autogenerate boilerplate for components and component unit tests. When creating a new
   component, this command can generate a new skeleton implementation of the component from the
   *Ai.xml file, saving significant development time. Providing the `--ut` flag will generate a unit
   testing skeleton for the component.
7. `check`: Builds, if necessary, then runs unit tests in the current directory. The `--all` runs all
   unit tests known to a deployment. The `--leak` flag will check for memory leaks while running the
   unit tests.
8. `coverage`: Similar to check, but calculates and generates unit test code coverage reports.
9. `new`: Creates either a new component or port. Use `--component` or `--port` to specify whether
    you want to create a component or port. If you would like to use a custom component
    cookiecutter, specify the template with `component-cookeicutter:` in the settings.ini.

Most `fprime-util` commands support a common set of flags that can be used to modify command
behavior:

- `-j JOBS, --jobs JOB`: set the number of parallel jobs while compiling. Defaults to 1, but it's
  recommended to set this to the number of cores on the machine + 1 to speed up compilation. Ex:
  `fprime-util build -j 10` will compile a project with 10 simultaneous jobs.
- `--all`: run the supplied command in a global deployment-level scope, rather than in the current
  directory. Ex: `fprime-util check` runs the unit tests in the current directory, `fprime-util
  check --all` runs every available unit test.
- `--ut`: apply the supplied command to the unit tests, rather than the default release. Ex:
  `fprime-util build --ut` will compile a project's unit tests.
- `-v, --verbose`: Produce verbose command output for debugging. Ex: `fprime-util build -v` will
  build a project with extra debug output.
- `-p PATH, --path PATH`: `fprime-util` defaults to running in the current working directory. The
  path flag allows users to override this behavior and manually set the directory `fprime-util`
  runs in. Ex: `fprime-util check -p SigGen` run `check` in the SigGen subdirectory.
- `-d DEPLOY, --deploy DEPLOY`: By default, `fprime-util` starts in the current working directory
  and starts moving up until it finds a F´ deployment. The deploy flag allows a user to override
  this behavior to manually specify a deployment. Ex: `fprime-util build -d ~/fprime` will try to
  build the `~/fprime` deployment.

## Setting Build Platform

Different F´ deployments will target different operating systems, architectures, and hardware. To
support a variety of different platforms, `fprime-util` allows specifying a target platform to
run against. By default, `fprime-util` will target the `native` platform and build for the host
computer's platform.

To cross-compile for a different platform than that host computer, the platform positional argument
can be passed to `fprime-util`.

For example, to cross-compile for the Raspberry Pi, the `raspberrypi` platform can be used:

```
$ fprime-util generate raspberrypi
$ fprime-util build raspberrypi
```

If a deployment is targeting a specific platform, the deployment's `settings.ini` file can be used
to override the default platform from `native` to a platform of your choosing. See [the
`settings.ini` guide](./settings.md) for more details.