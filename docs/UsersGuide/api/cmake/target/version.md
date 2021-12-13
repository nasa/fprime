**Note:** auto-generated from comments in: ./target/version.cmake

## Dict function `add_module_target`:

Adds a module-by-module target for producing dictionaries. These dictionaries take the outputs
from the autocoder and copies them into the correct directory. These outputs are then handled as
part of the global `dict` target above.


- **MODULE_NAME:** name of the module
- **TARGET_NAME:** name of target to produce
- **GLOBAL_TARGET_NAME:** name of produced global target
- **AC_INPUTS:** list of autocoder inputs
- **SOURCE_FILES:** list of source file inputs
- **AC_OUTPUTS:** list of autocoder outputs
- **MOD_DEPS:** module dependencies of the target


