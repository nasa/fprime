# Configuring an IDE for Use With F´

Many projects like to use IDEs for development.  This makes development, testing, and tracing easier.  This document
contains some basic instructions on how to set up an IDE.  Hopefully, the instructions will work for the IDE of your
choice, however; some specific examples are shown below.

- [Basic IDE Setup: CMake and Generation Variables](#basic-ide-setup-cmake-and-variables)
- [IDE Examples](#ide-examples)
    - [CLion](#clion)
    - [Eclipse and Eclipse Variants](#eclipse-and-eclipse-variants)
    - [VS Code](#vs-code)
- [Conclusion](#conclusion)

## Basic IDE Setup: CMake and Variables

F´ typically builds by hand using the F´ utility. This utility automates away many of the details of setting up an F´
build and should make it easy for users to build, run, and test F´ from the command-line.  If this approach works for
your project, we recommend you use it.  However, if you need IDE support these instructions are for you.

This guide will walk you through the setup and build of an F´ application through your IDE's CMake plugin.  For helper
tasks like generating templated files, it is probably best to just continue using `fprime-util`, but for coding
tasks like building, running UTs, generating indices for the IDE's helper functions, debugging, etc, we will attempt to
setup the IDE correctly.

**Note:** although it may be possible to integrate your IDE directly with the `fprime-util` build caches, we will not
attempt this here as it may result in many errors.

### Step 1: CMake Support for Our Beloved IDE

F´ is built using CMake and thus to integrate with your IDE, your project must be able to support CMake builds. Some IDEs
support this natively, and some require plugins. Apart from the examples below, you should understand in general how
your beloved IDE integrates with CMake builds and how to configure CMake cache variables like setting a toolchain file.

### Step 2: Identifying F´ Build Information

The CMake requires that several build settings are set.  The `fprime-util` sets these automatically from the
`settings.ini` file, however; here we will need to configure this by hand as we are using CMake directly.

Typically a user wants to build a `Testing` build for the default toolchain using an internal-to-F´ branch. If you are
just learning F´, use these settings to run and test on your personal computer hardware and skip to Step 3.

First, the user should determine whether they would like a `Testing`, `Release`, or `Debug` build.  The user should
typically choose `Testing`, which allows for running unit tests, enables debug flags, and links in all of the test code.
`Release` and `Debug` builds are provided by default by CMake but are used by F´ only to create the executable and not
the test setup.

Next, the user should choose a toolchain file. If you are building for your host system to debug or test a native
executable just choose the default. If you must run on a target platform and want to build for it, choose the toolchain
file you would use to build for that platform.

Third, the user should determine if they are using an internal-to-F´ build, or an external-to-F´ build.  If your code is
placed within the F´ framework checkout (code supplied by F´ team), you are using the former.  If your code specifies
where to find F´ code and lives outside the F´ framework directory you are using the latter.

Finally, identify any other build options or cache variables you need. These are anything you supply to the build in
the form of -D options. This can include custom configuration setups.

### Step 3: Setting Up CMake and Build Variables

Through the setup of the IDE, you should now set the following variables.  Note that some of these settings are exposed by
the IDE as deliberate settings, but they can be set as part of the build setup.

| Variable | Value | Typical |
|----------|-------|---------|
| -DCMAKE_BUILD_TYPE | Testing, Debug, or Release | Testing |
| | | |
| **Custom Toolchain**       | | |
| -DCMAKE_TOOLCHAIN_FILE     | path/to/toolchain/file      | **unneeded** |
| | | |
| **Out of Source Builds**   | |
| -DFPRIME_FRAMEWORK_PATH    | path/to/fprime/framework    | **unneeded** |
| -DFPRIME_PROJECT_ROOT      | path/to/project/root        | **unneeded** |
| -DFPRIME_LIBRARY_LOCATIONS | path/to/lib:path/to/lib:... | **unneeded** |
| | | |
| **Custom Configuration**   | | |
| -DFPRIME_AC_CONSTANTS_FILE | path/to/project/ac/file     | **unneeded** |
| -DFPRIME_CONFIG_DIR        | path/to/project/conf/dir    | **unneeded** |

**Note:** See [CMake Settings](cmake-settings.md) for a further description of these and other settings.

## IDE Examples

IDEs change all the time and configuring them is not something a project like F´ should take on as a documentation task
as the overhead to support this would be immense. However, in so much as IDEs are used by the F´ team and community, we
will provide this section to give users and developers the most help possible even if it is not complete.

**Note:** ensure that you understand the information in [basic IDE setup](#basic-ide-setup-cmake-and-variables). When
IDEs change, it is typically not too hard to configure them from an old example and an understanding of what settings
are needed.

### CLion

Note: this is not written yet.

### Eclipse and Eclipse Variants

Note: this is not written yet.

### VS Code

Note: this is not written yet.

## Conclusion

In this guide, we looked at setting up the most basic IDE settings.  You should now see all of our CMake targets and be
able to run and debug them.  More advanced usage is likely documented within the IDE's documentation.