**Note:** auto-generated from comments in: ./autocoder/helpers.cmake

## autocoder/helpers.cmake:

Helper functions to make implementing custom autocoders easier! These codify some of the basic patterns for autocoders
so that implementers can handle standard functions easily.


## Macro `autocoder_support_by_suffix`:

This sets up an autocoder to handle files based on a suffix. For example, passing in "*.fpp" will support all files
ending in ".fpp" i.e. FPP files or passing in "ComponentAi.xml" will support all component XMLs. It is implemented as
a macro such that users need not do anything other than call it with the suffix to setup the system correctly. This
performs raw ascii comparison, not regular expression matching.

**Note:** this will set the appropriate variable in PARENT_SCOPE and since it is macro this will be the parent scope
of the caller.

**SUFFIX**: suffix to support
**AC_INPUT_FILE**: file to check with suffix
**REQUIRE_CMAKE_RESCAN:** (optional) this file should trigger a cmake rescan. Default: false


## Function `requires_regeneration`:

Called by the autocoder when a source file needs to setup CMake to reconfigure when the source file changes.

`AC_INPUT_FILE`: file to mark as tracked


## Function `_set_autocoder_name`:

Function to set AUTOCODER_NAME in parent scope. Helper to the below two function, not intended for users to call this
function.

FUNCTION_NAME: function name for producing error message


## Macro `autocoder_setup_for_individual_sources`:

A helper to setup the autocoder to handle individual sources. Each source input to the autocoder will result in a new
call through the autocoder. This handles setting the appropriate property such that this autocoder need not know nor
care about the property itself. This may only be called from within an autocoder file.

function(autocoder_setup_for_individual_sources)
    _set_autocoder_name(autocoder_setup_for_individual_sources)
    set_property(GLOBAL PROPERTY "${AUTOCODER_NAME}_HANDLES_INDIVIDUAL_SOURCES" TRUE)
endfunction()



