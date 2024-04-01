**Note:** auto-generated from comments in: ./autocoder/autocoder.cmake

## autocoder/autocoder.cmake:

Autocoder setup and support file. This performs all of the general autocoder functions, running the specific functions
defined within the individual autocoders. This gives the ability to run a set of autocoders to produce files.

Note: autocoders need to be run by targets. See target/target.cmake.


## run_ac_set:

Run a set of autocoder allowing back-to-back execution of a set of autocoders. SOURCES are the source files that are
input into the autocoder filters.  Extra arguments may be an include path for an autocoder (e.g. autocoder/fpp).

SOURCES: source file input list
...: autocoder include


## run_ac:

Run the autocoder across the set of source files, SOURCES, and the previously generated sources, GENERATED_SOURCES.
This will filter the SOURCES and GENERATED_SOURCES down to the handled set. Then for single-input autocoders, it runs
the autocoder one input at a time, otherwise it runs the autocoder once on all inputs.

AUTOCODER_CMAKE: cmake file containing autocoder definition
SOURCES: sources input to run on the autocoder
GENERATED_SOURCES: sources created by other autocoders


## Function `_describe_autocoder_prep`:

Describes the inputs into an autocoder run. Does nothing unless CMAKE_DEBUG_OUTPUT is ON. Run before running the
autocoder

AUTOCODER_NAME: name of autocoder being run
AC_INPUT_SOURCES: input files to autocoder


## Function `_describe_autocoder_run`:

Describe the results of an autocoder run. Does nothing unless CMAKE_DEBUG_OUTPUT is ON. Must have run the autocoder
already and set the properties.

AUTOCODER_NAME: name of autocoder being described


## _filter_sources:

Filters sources down to the ones supported by the active autocoder. It is an error to call this helper function before
including an autocoder's CMake file and thus setting the active autocoder. Helper function.

OUTPUT_NAME: name of output variable to set in parent scope
GENERATED_SOURCES: sources created by other autocoders
...: any number of arguments containing lists of sources


## __ac_process_sources:

Process sources found in SOURCES list and sets up the autocoder to run on the sources by registering a rule to create
those sources.
SOURCES: source file list. Note: if the autocoder sets HANDLES_INDIVIDUAL_SOURCES this will be singular


