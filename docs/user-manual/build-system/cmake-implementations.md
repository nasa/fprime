# CMake Implementations

Certain parts of F´ have different implementations that a project may choose from.  The canonical example of this is a file system implementation. Projects may wish to use the stubbed implementation of the file system, the platform supplied standard implementation, or something project-specific like a flash chip or SD card.

These packages must be fully specified. Thus, every platform must specify a specific implementation for every package. These specific implementations may be overridden for a specific executable deployment or unit test.

## Requiring an Implementation

Modules that require an implementation should use the directive `REQUIRES_IMPLEMENTATIONS` in the `register_fprime_module` call.

> [!NOTE]
> `REQUIRES_IMPLEMENTATIONS` is only needed for modules that have a direct dependency on the implementation, not on their dependents.

## Choosing an Implementation

Platform developers *must* specify an implementation of every package used in the system. Failing to do so means that a given functionality is undefined and impossible to link.  Stub implementations are provided in the case that a platform does not support a given package's functionality.

Choosing an implementation is done with the `CHOOSES_IMPLEMENTATIONS` directive available to `register_fprime_config`. Platform developers should choose implementations as part of the platform definition.

https://github.com/nasa/fprime/blob/dfaf496263bdfff04461179eb99fb3f906a10009/cmake/platform/Linux.cmake#L15-L17

## Overriding an Implementation Choice

Executables, unit tests, and deployments may wish to use a different implementation than that specified by the platform. This can be done by using `CHOOSES_IMPLEMENTATIONS` directive call in the deployment, executable, or unit test's registration. For example, a unit test may wish to choose `Os_File_Stub` as an implementation of `Os_File` to disable platform file system support for the given unit test:

```
register_fprime_ut(
    ...
  CHOOSES_IMPLEMENTATIONS
    Os_File_Stub
)
```

> [!NOTE]
> A CMake target with the name of the chosen implementor *must* be defined somewhere in F´, an F´ library used by the project, or by the project itself.

## Defining an Implementation

To define an implementation, developers should declare an F´ module (using `register_fprime_module`) that implements the needed interface of the package and supply the `IMPLEMENTS` directive to that call.

## Conclusion

F´ provides for various implementations for certain packages needed by the framework.
