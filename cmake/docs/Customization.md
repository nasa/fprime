# CMake Customization

Some projects may wish to customize the CMake system in order to perform some things that the
standard CMake and F prime systems do not allow. This guide is designed to provide some basic
advice on how to configure standard customization items as part of this CMake system.

**Important** this system is CMake and that give the user much power. Just about anything desired
can be done using standard CMake patterns. Thus, the user is encouraged to study CMake if advice
cannot be found herein.

## Build F Prime Utilites

Adding a utility executable that dependns on F prime code is easy. Just perform a standard call to
`register_fprime_executable`. Care should be taken to set the variable `EXECUTABLE_NAME` before
making this call in order to set the utility's name. This executable will then be output as part
of the deployment's build. A separate tools deployment may be used to build only utilities.

See: [API](API.md)

## Cutsom Build-System Commands (Make Targets)

In order to add a custom build-system command (think make target) the CMake call to add a target
can be used. This target can then have dependencies added to it and can be run after CMake is setup
by running the standard build command supplying this target.

**Example:**

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
