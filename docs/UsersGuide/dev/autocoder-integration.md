# Integrating a New Autocoder

This guide will walk the user through integrating a new autocoder into the F´ build system. In the most basic form an
autocoder consists of several elements: input models, output generated code, steps to genenerate output from the given
input, and a trigger to start the process. As a byproduct, this process also produces "module dependencies" and
"file dependencies" used to tie the autocoder into the make system itself.

## Autocoder CMake Structure

An autocoder needs to implement a CMake file that will be used to run the autocoder itself. Within this file the
developer is required to implement several functions and set several variables to tell the system how to run the
autocoder and setup the build trigger. Each function needed will be expanded on below, with examples.

The CMake system is designed to take notes of the returned files and dependencies as shown below. This helps ensure that
the autocoder is only rerun when it needs to.  To support this, a memo file is created to track the outputs of the
functions defined below.

Once the autocoder file is constructed, the autocoder needs to be registered to modules. See:
[target-integration](./target-integration.md).

## Required Function `is_supported`

This function takes a single input file and determines whether it can be processed by this autocoder. This function is
required to set `IS_SUPPORTED` to `TRUE` or `FALSE` in the parent scope indicating its ability to handle the given
input file. All matching inputs are provided to the other functions of this API as input.

The following example reports that any file ending in `Ai.xml` may be processed by this autocoder.

```cmake
function(is_supported AC_INPUT_FILE)
    set(IS_SUPPORTED FALSE PARENT_SCOPE)
    if (AC_INPUT_FILE MATCHES ".*Ai\\.xml")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    endif()
endfunction (is_supported)
```

## Required Function `get_generated_files`

This function takes a list of input files and determines the files that will be produced when this autocoder is run.
This function is required to set `GENERATED_FILES` in the parent scope to hold this list of produced files. The list of
input files will singular if the current autocoder sets `HANDLES_INDIVIDUAL_SOURCES` to `TRUE` and this function will
be called once for each matching input.

**Note:** it is sometimes efficient to also set `MODULE_DEPENDENCIES` and `FILE_DEPENDENCIES` in this call and merely
verify they were set in the call to `get_dependencies`.

The following example will generate a <input>.ac file for each input file supplied. It assumes
`HANDLES_INDIVIDUAL_SOURCES` is set to `FALSE`.

```cmake
function(get_generated_files AC_INPUT_FILE)
    set(GENERATED_FILES)
    foreach(INPUT_FILE IN LISTS AC_INPUT_FILE)
        list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${AC_INPUT_FILE}.ac")
    endforeach()
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
endfunction(get_generated_files)
```

## Required Function `get_dependencies`

This function takes a list of input files and determines two sets of dependencies to determine if an when the autocoder
needs to be run again: `MODULE_DEPENDENCIES` and `FILE_DEPENDENCIES`. `MODULE_DEPENDENCIES` are fprime modules that,
when updated or rebuilt, will require this autocoder to be run again. `FILE_DEPENDENCIES` are files that, when changed,
will require the autocoder to be run again. Both are set in parent scope. The list of  input files will singular if the
current autocoder sets `HANDLES_INDIVIDUAL_SOURCES` to `TRUE` and this function will be called once for each matching
input.

The following example lists only `Fw_Port` as a dependent module and lists no file dependencies.

```cmake
function(get_dependencies AC_INPUT_FILE)
    set(MODULE_DEPENDENCIES Fw_Port PARENT_SCOPE)
    set(FILE_DEPENDENCIES "" PARENT_SCOPE)
endfunction(get_dependencies)
```

## Required Function `setup_autocode`

This function takes a list of input files, list of generated files, list of module dependencies, list of file
dependencies and an extras variable. It should setup the necessary commands to run the autocoder. This is usually done
with `add_custom_command`. The list of input files will singular if the current autocoder sets
`HANDLES_INDIVIDUAL_SOURCES` to `TRUE` and this function will be called once for each matching input.

The following example creates the generated functions using CMake's touch function.

```cmake
function(setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    add_custom_command(
            OUTPUT  ${GENERATED_FILES}
            COMMAND ${CMAKE_COMMAND} -E touch ${GENERATED_FILES}
            DEPENDS ${AC_INPUT_FILE} ${MODULE_DEPENDENCIES} ${FILE_DEPENDENCIES}
    )
endfunction(setup_autocode)
```

## Optional Variable `HANDLES_INDIVIDUAL_SOURCES`

Set `HANDLES_INDIVIDUAL_SOURCES` at file-scope to indicate that the autocoder will treat source files individually and
each supplied input file list will be singular. The default behavior is `TRUE`.

The following example sets it to false.
```cmake
set(HANDLES_INDIVIDUAL_SOURCES FALSE)
```

## Optional Function `regenerate_memo`

This function takes an output variable, memo file path, and a list of source files. It should set the OUTPUT variable in
parent scope to true if the memo file needs regeneration and false otherwise. The default behavior is to regenerate the
memo file if it does not exist or the input sources have changed.

In this example, the memo will be set to regenerate if the memo file does not exist. 

```cmake

function(regenerate_memo OUTPUT MEMO_FILE SOURCES_INPUT)
    set(${OUTPUT} FALSE PARENT_SCOPE)
    if (NOT EXISTS ${MEMO_FILE})
        set(${OUTPUT} TRUE PARENT_SCOPE)
    endif()
endfunction(regenerate_memo)
```

## Full Example

This is all the example code in one block for convenience.

```cmake
set(HANDLES_INDIVIDUAL_SOURCES FALSE)

function(is_supported AC_INPUT_FILE)
    set(IS_SUPPORTED FALSE PARENT_SCOPE)
    if (AC_INPUT_FILE MATCHES ".*Ai\\.xml")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    endif()
endfunction (is_supported)

function(get_generated_files AC_INPUT_FILE)
    set(GENERATED_FILES)
    foreach(INPUT_FILE IN LISTS AC_INPUT_FILE)
        list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${AC_INPUT_FILE}.ac")
    endforeach()
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
endfunction(get_generated_files)

function(get_dependencies AC_INPUT_FILE)
    set(MODULE_DEPENDENCIES Fw_Port PARENT_SCOPE)
    set(FILE_DEPENDENCIES "" PARENT_SCOPE)
endfunction(get_dependencies)

function(setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    add_custom_command(
            OUTPUT  ${GENERATED_FILES}
            COMMAND ${CMAKE_COMMAND} -E touch ${GENERATED_FILES}
            DEPENDS ${AC_INPUT_FILE} ${MODULE_DEPENDENCIES} ${FILE_DEPENDENCIES}
    )
endfunction(setup_autocode)

function(regenerate_memo OUTPUT MEMO_FILE SOURCES_INPUT)
    set(${OUTPUT} FALSE PARENT_SCOPE)
    if (NOT EXISTS ${MEMO_FILE})
        set(${OUTPUT} TRUE PARENT_SCOPE)
    endif()
endfunction(regenerate_memo)
```