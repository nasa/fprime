# How-To: Develop an F´ Library

This guide will walk you through the structure and best practices in developing an F´ library.  F´ libraries are used to share modules (components, ports, topologies, etc.), toolchains, and platforms between F´ projects.  These libraries help with code reuse between projects by enabling direct sharing of F´ software.

*Contents:*
1. [F´ Library Structure](#flibrary-structure)
2. [Required: `library.cmake`](#required-librarycmake)
3. [Optional: Toolchain Folder and Toolchain Files](#optional-toolchain-folder-and-toolchain-files)
4. [Optional: Platform Folder and Platform Files](#optional-platform-folder-and-platform-files)
5. [Optional: F´ Module Directories](#optional-fmodule-directories)

## F´ Library Structure

In this section, you will learn about the expected structure of an F´ library. An F´ library must have a `library.cmake` and may have any of the following items:

1. Module Directories and Modules (Components, Ports, Topologies, etc.)
2. `cmake/toolchain` Folder and Toolchain Files
3. `cmake/platform` Folder and Platform Files

That means that a complete F´ library might look like the following:

```bash
my-library/
├── cmake
│   ├── platform
│   └── toolchain
├── MyLibrary
│   ├── Components
│   │   └── MyComponent
│   │       └── ...
│   └── MyDeployment
│       └── ...
└── library.cmake
```

> Module directories shall be namespaced (i.e. `MyLibrary` above) to ensure that modules do not collide with F´ and other libraries. Placing container directories (e.g. `Components`, `Svc`, `Drv`, etc.) directly at the root of the repository is *strongly* forbidden.

You will see how to develop each of these parts of the library in the following sections.

##  Required: `library.cmake`

The `library.cmake` file is the only required file for an F´ library.  It consists of a series of `add_fprime_subdirectory` calls that include the various modules that are part of the library.  For libraries that just contain toolchains and platforms, this file may be empty. 

**Sample `library.cmake`**
```cmake
####
# library.cmake for MyLibrary
####
add_fprime_subdirectory("${CURRENT_CMAKE_LIST_DIR}/MyLibrary/Components/MyComponent")
add_fprime_subdirectory("${CURRENT_CMAKE_LIST_DIR}/MyLibrary/MyTopology")
```

`library.cmake` should exist at the root of the library's directory structure.


## Optional: F´ Module Directories

F´ libraries share F´ code through modules. These module directories are created by `fprime-util new --component` or by hand and may include any F´ code (Components, Ports, Topologies, Data Types, etc.).  These types are made available to users of the library by ensuring that they are added to the `library.cmake` file.  The only restriction is that these component should be placed under a namespacing directory for the library (e.g. `MyLibrary`) to avoid collisions with other libraries and F´ code.

Developing component, ports, topologies, etc. are the subject of our [tutorials](../Tutorials/README.md).


## Optional: Toolchain Folder and Toolchain Files

F´ libraries often need to supply new toolchain integrations with F´. These libraries must define a `cmake/toolchain` folder and place toolchain files within that folder.  Any file ending in `.cmake` and placed within the `cmake/toolchain` folder will be available as a toolchain via `fprime-util generate <toolchain>`.

The example toolchain above (`cmake/toolchain/my-toolchain`) would be found as `my-toolchain` in `fprime-util`.

```bash
fprime-util generate my-toolchain
```

The `cmake/toolchain` folder may contain any number of toolchains and must be placed in the root of the library's directory structure.

## Optional: Platform Folder and Platform Files

In a similar manner to toolchains, platforms may be provided in the `cmake/platform` folder. Any file ending in `.cmake` and placed within the `cmake/platform` folder will be available to F´  to be loaded as a platform. Commonly, this means that a toolchain would include the line `set(FPRIME_PLATFORM <platform>)`.

The example platform would thus need to include `set(FPRIME_PLATFORM "my-platform")`.

The `cmake/platform` folder may contain any number of platform files and must be placed in the root of the library's directory structure.

## Conclusion

You should now understand the structure of an F´  library an know how to assemble one.
