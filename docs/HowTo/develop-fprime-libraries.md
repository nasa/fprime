# How-To: Develop and F´ Library

This guide will walk you through the structure and best practices in developing an F´ library.  F´ libraries are used to share modeules (components, ports, topologies, etc.), toolchains, and platforms between F´ projects.  These libraries help with code reuse between projects by enabling direct sharing of F´ software.

## F´ Library Structure

In this section, you will learn about the expected strcture of an F´ library. An F´ library must have a `library.cmake` and may have any of the following items:

1. `cmake/toolchain` Folder and Toolchain Files
2. `cmake/platform` Folder and Platform Files
3. Module Directories and Modules (Components, Ports, Topologies, etc.)

That means that a complete F´ library might look like the following:

```bash
library.cmake
cmake/toolchain/my-toolchain.cmake
cmake/platform/my-platform.cmake
MyLibrary/Components/MyComponent/...
MyLibrary/MyTopology/...
```

> Module directories shall be namespaced (i.e. `MyLibrary` above) to ensure that modules do not collide with F´ and other libraries. Placing directories like `Svc` directly at the root of the repository is *strongly* discouraged.

You will see how to develop each of these parts of the library in the following sections.

## 
