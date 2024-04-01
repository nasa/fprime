**Note:** auto-generated from comments in: ./autocoder/ai-shared.cmake

## `autocoder/ai-shared.cmake`:

Shared implementation for AI XML files. Contains helper and base functions for writing those autocoder more easily.


## Function `ai_split_xml_path`:

Splits the XML path into the object type (component, port, ...) and the name (e.g. SignalGen) for consumption in
various ai autocoders.

`AC_INPUT_FILE`: path to ai file. Need not exist.
OUTPUTS: XML_LOWER_TYPE, XML_TYPE, OBJ_TYPE


## Macro `ai_shared_setup`:

Get the shared setup for the ai autocoder (e.g. codegen invocation). Used in multiple ai autocoders.

OUTPUT_DIR: directory to place outputs into
Returns: AI_BASE_SCRIPT, configured codegen call.


