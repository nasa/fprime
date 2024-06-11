# Package Implementations

Certain parts of F´ have different implementations that a project may choose from.  The canonical example of this is a file system implementation. Projects may wish to use the stubbed implementation of the file system, the platform supplied standard implementation, or something project-specific like a flash chip or SD card.

These packages must be fully specified. Thus, every platform must specify a specific implementation for every package. These specific implementations may be overridden for a specific executable deployment or unit test.

## Requiring an Implementation

Modules that require an implementation should call the CMake API function `require_fprime_implementation()` to declare a requirement on a specific implementation. For example, the Os module `require_fprime_implementation(Os/Task)` declares that some implementation of `Os/Task` is required.

> Only `Os` requires an implementation of `Os/Task` as users of the `Os` package get the implementation through `Os` and should not specify it directly.

## Choosing an Implementation

Platform developers *must* specify an implementation of every package used in the system. Failing to do so means that a given functionality is undefined and impossible to link.  Stub implementations are provided in the case that a platform does not support a given package's functionality.

Choosing an implementation is done with a series of `choose_fprime_implementation` calls. One for each required implementation defined in the system.  These calls are put in a platform CMake file.

## Overriding an Implementation Choice

Some executables, unit tests, and deployments may wish to use a different implementation than that specified by the platform. This can be done by a `choose_fprime_implementation` call in the deployment, executable, or unit test's module. For example, a unit test may wish to choose `Os_File_Stub` as an implementation of `Os_File` to disable platform file system support for the given unit test.

> A CMake target with the name of the chosen implementor *must* be defined somewhere in F´, an F´ library used by the project, or by the project itself.

## Defining an Implementation

To define an implementation, developers should declare an F´ module (using `register_fprime_module`) that implements the needed interface of the package.  Then call `register_fprime_implementation` to specify this module is an implementation.

## Conclusion

F´ provides for various implementations for certain packages needed by the framework.  See `cmake/API` and `Os` for API descriptions and Os implementations.
