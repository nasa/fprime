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
4.
