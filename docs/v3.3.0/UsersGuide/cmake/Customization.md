# CMake Customization

Some projects may wish to customize the CMake system in order to perform some things that the
standard CMake and F prime systems do not allow. This guide is designed to provide some basic
advice on how to configure standard customization items as part of this CMake system.

**Important** this system is CMake and that gives the user much power. Just about anything desired
can be done using standard CMake patterns. Thus, the user is encouraged to study CMake if advice
cannot be found herein.

## Build F Prime Utilities

Adding a utility executable that depends on F prime code is easy. Just perform a standard call to
`register_fprime_executable`. Care should be taken to set the variable `EXECUTABLE_NAME` before
making this call in order to set the utility's name. This executable will then be output as part
of the deployment's build. A separate tools deployment may be used to build only utilities.

See: [API](API.md)

## Custom Build-System Commands (Make Targets)

Custom build targets that need to be built against modules and global targets can be generated
using the hook pattern. This pattern involves creating a file with two functions - `add_global_target` and `add_module_target`. These functions are called to add targets to the top level and each module respectively.
Then this file is registered with `register_fprime_target`.

In the two add functions, the user is expected to call the `add_custom_target` CMake command in order to compose the target itself. **Note:** the user may simply call this function directly to
add targets that don't have both per-module and global steps.

See:
  - [add_custom_target](https://cmake.org/cmake/help/latest/command/add_custom_target.html) to view
information on CMake targets.
  - [Target](support/Target.md) for the format of the target file
  - [API](API.md) for the syntax of the register call
  - [dict.cmake](../target/dict.cmake) as an example of adding targets

**Example (Raw Global Target):**

```
add_custom_target(
            dict
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/dict/serializable ${CMAKE_SOURCE_DIR}/py_dict/serializable
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/py_dict/serializable/__init__.py
        )
```
**Running Example:**

```
cmake ../Ref
make dict
```

## Custom External Libraries With Other Build Systems

There are two ways to handle external libraries and build them within CMake. One is to simply
use the `add_directory` cmake command to add the directory. Then use a `CMakeLists.txt` to call
`add_custom_target` or `add_custom_command` trigger the actual building. The `add_custom_target`
is used when the system does not depend on the files produced (directly) but may need a link
dependency, and `add_custom_command` is used when the system needs access to the output files.

Alternatively, `ExternalProject_Add` can be used if the external library requires download,
version control, and building steps.

**Supporting Documentation:**
1. [https://cmake.org/cmake/help/latest/command/add_custom_target.html](https://cmake.org/cmake/help/latest/command/add_custom_target.html)
2. [https://cmake.org/cmake/help/latest/command/add_custom_command.html](https://cmake.org/cmake/help/latest/command/add_custom_command.html)
3. [https://cmake.org/cmake/help/latest/module/ExternalProject.html](https://cmake.org/cmake/help/latest/module/ExternalProject.html)
4. [https://blog.kitware.com/wp-content/uploads/2016/01/kitware_quarterly1009.pdf](https://blog.kitware.com/wp-content/uploads/2016/01/kitware_quarterly1009.pdf)