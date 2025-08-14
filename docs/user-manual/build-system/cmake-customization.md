# CMake Customization

Some projects may wish to customize the CMake system in order to perform some things that the
standard CMake and F prime systems do not allow. This guide is designed to provide some basic
advice on how to configure standard customization items as part of this CMake system.

**Important** this system is CMake and that gives the user much power. Just about anything desired
can be done using standard CMake patterns. Thus, the user is encouraged to study CMake if advice
cannot be found herein.

## Build F Prime Utilities

Adding a utility executable that depends on F prime code is easy. Just perform a standard call to
`register_fprime_executable`. Care should be taken to set the executable name as the first argument to that
call. This executable will then be output as part of the deployment's build and may be built directly by name.
A separate tools deployment may be used to build only utilities.

See: [API](cmake-api.md)

## Custom Build-System Commands (Make Targets)

Custom build targets that need to be built against modules and global targets can be generated
using the hook pattern. This pattern involves creating a file with three functions - `<target>_add_global_target`, `<target>_add_module_target`, and `<target>_add_deployment_target`. These functions are called to add targets to the top level and each module respectively.
Then this file is registered with `register_fprime_target`.

These functions can have any code the target needs, but typically uses `add_custom_target` to register the actual target.

See:
  - [add_custom_target](https://cmake.org/cmake/help/latest/command/add_custom_target.html) to view
information on CMake targets.
  - [API](cmake-api.md) for the syntax of the register call
  - [Targets](cmake-targets.md) for information on the built-in targets

## Custom External Libraries With Other Build Systems

There are two ways to handle external libraries and build them within CMake. One is to simply
use the `add_directory` cmake command to add the directory. Then use a `CMakeLists.txt` to call
`add_custom_target` or `add_custom_command` trigger the actual building. The `add_custom_target`
is used when the system does not depend on the files produced (directly) but may need a link
dependency, and `add_custom_command` is used when the system needs access to the output files.

Alternatively, `ExternalProject_Add` can be used if the external library requires download,
version control, and building steps.

For a guide on integrating with another build system see:
[How To: Integrate External Libraries](../../how-to/integrate-external-libraries.md).