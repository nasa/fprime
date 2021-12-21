**Note:** auto-generated from comments in: ./target/dict.cmake

## dict.cmake:

Dictionary target definition file. Used to define `dict` and `<MODULE>_dict` targets. Defined as
a standard target pattern. This means that the following functions are defined:

- `add_module_target`: adds sub-targets for '<MODULE_NAME>_dict'


## Dict function `add_module_target`:

Process the dictionary target on each module that is defined. Since the topology module is going to do the dictionary
generation work for us, we just need to add a dependency on the module that contains the dictionary in its list of
autocoder output files.

- **MODULE:** name of the module
- **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
- **SOURCE_FILES:** list of source file inputs from the CMakeList.txt setup
- **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt


