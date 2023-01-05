**Note:** auto-generated from comments in: ./target/dict.cmake

## dict.cmake:

Dictionary target definition file. Used to define `dict` and `<MODULE>_dict` targets. Defined as
a standard target pattern. This means that the following functions are defined:

- `add_global_target`: adds a global target 'dict'
- `add_module_target`: adds sub-targets for '<MODULE_NAME>_dict'


## Function `dict`:

Generate a dictionary from any *AppAi.xml file that we see


## Dict function `add_global_target`:

Add target for the `dict` custom target. Dictionaries are built-in targets, but they are defined
as custom targets. This handles the top-level dictionary target `dict` and registers the steps to
perform the generation of the target.  TARGET_NAME should be set to `dict`.

- **TARGET_NAME:** target name to be generated


## Dict function `add_module_target`:

Adds a module-by-module target for producing dictionaries. These dictionaries take the outputs
from the autocoder and copies them into the correct directory. These outputs are then handled as
part of the global `dict` target above.


- **MODULE_NAME:** name of the module
- **TARGET_NAME:** name of target to produce
- **AC_INPUTS:** list of autocoder inputs
- **SOURCE_FILES:** list of source file inputs
- **AC_OUTPUTS:** list of autocoder outputs
- **MOD_DEPS:** module dependencies of the target


