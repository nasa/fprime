**Note:** auto-generated from comments in: ../../support/Utils.cmake

## Get Module Name:

Takes a path, or something path-like and returns the module's name. This breaks down as the
following:

 1. If passed a path, the module name is the _ed variant of the relative path from BUILD_ROOT
 2. If passes something which does not exist on the file system, it is just _ed

i.e. ${BUILD_ROOT}/Svc/ActiveLogger becomes Svc_ActiveLogger
     Svc/ActiveLogger also becomes Svc_ActiveLogger

\param DIRECTORY_PATH: path to infer MODULE_NAME from
\param UT: "TRUE" if unit-test varian "FALSE" otherwise
\return MODULE_NAME (set in parent scope


## Setup Module Dicts:

Creates a dictionary target for the module, that is then added to the "dict" and "module"
targets.
\param MOD_NAME: name of module being processed
\param AI_XML: AI_XML that is generating dictionaries
\param DICT_INPUTS: inputs from auto-coder, used to trigger dictionary generation


## Add Dict Deps:

Used to track dictionary dependencies, in order to ensure that the fewest number of dictionary
targets are used.
\param MODULE_NAME: module receiving a dict dependency
\param DEP_MODULE_NAME: name of the module whose dictionary will be added


## Generated Files:

Add generated file as sources for the given target. This function specifically adds
these sources as generated, so that regardless of where they appear (source or build tree)
they will be known as generated

\param CPP_SOURCE: source .cpp file to add
\param HPP_SOURCE: source .hpp file to add
\param MODULE_NAME: name of the module to add generated sources too


## FPrime Dependencies:

A function used to detect the dependencies of a given module from the XML file that
defines this module. This is used to reduce code in the Serializable, Port, Component,
and Topology functions that all use the same procedure.

\param XML_PATH: full path to the XML used for sources.
\param MODULE_NAME: name of the module soliciting new dependencies
\param PARSER_TYPE: type of parser to use. Must be one of the prefixes *_xml in cmake/parser/


## split_source_files:

Split SOURCE_FILE_INPUTS into the autocoder and source variants, supplied to the calling function


## split_dependencies:

Split DEPS_INPUT into the module and link variants, supplied to the calling function.


## Print Dependencies:

Prints the dependency list of the module supplied as well as the include directories.


