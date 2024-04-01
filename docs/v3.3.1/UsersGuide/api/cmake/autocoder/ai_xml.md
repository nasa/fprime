**Note:** auto-generated from comments in: ./autocoder/ai_xml.cmake

## autocoder/ai-xml.cmake

Primary Ai XML autocoder that sets up the C++ file generation. This is a implementation of the singular autocoder
setup of the original CMake system. It process AI XML files and produces the autocoder output files Ac.hpp and
Ac.cpp files.


## `ai_xml_is_supported`:

Required function, processes ComponentAi.xml files.
`AC_INPUT_FILE` potential input to the autocoder
...: any number of arguments representing a list of previously generated files


## Function `__ai_info`:

A function used to detect all the needed information for an Ai.xml file. This looks for the following items:
 1. Type of object defined inside: Component, Port, Enum, Serializable, TopologyApp
 2. All fprime module dependencies that may be auto-detected
 3. All file dependencies

- **XML_PATH:** full path to the XML used for sources.
- **MODULE_NAME:** name of the module soliciting new dependencies


## `ai_xml_setup_autocode`:

Required function, sets up a custom command to produce Ac.hpp and Ac.cpp files.


