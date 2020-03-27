**Note:** auto-generated from comments in: ../module-CMakeLists.txt.template

## Module 'CMakeLists.txt':

F prime modules setup source files, non-standard module dependencies, unit test source files,
unit test module dependencies, and call one of the following functions from the F prime API:

Functions:
- register_fprime_module: register a typical module/library
- register_fprime_executable: register an executable/deployment binary
- register_fprime_ut: register a unit test

These functions are powered off the following variables:
- `SOURCE_FILES:` combined list of source and autocoding files used in `register_fprime_module` and
`register_fprime_executable` to provide sources.
- `MOD_DEPS:` (optional) module dependencies that cannot be auto-detected, used in
`register_fprime_module` and `register_fprime_executable`.
- `EXECUTABLE_NAME`: (optional) override default executable name in `register_fprime_executable`
- `UT_SOURCE_FILES`: (optional) specify sources for unit tests used with `register_fprime_ut`
- `UT_MOD_DEPS`: (optional) specify modules for unit tests used with `register_fprime_ut`

See: [API.md](API.md)

**Example:**
```
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherImpl.cpp"
)

register_fprime_module()
set(UT_SOURCE_FILES
  "${FPRIME_CORE_DIR}/Svc/CmdDispatcher/CommandDispatcherComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherImplTester.cpp"
)
register_fprime_ut()
```


