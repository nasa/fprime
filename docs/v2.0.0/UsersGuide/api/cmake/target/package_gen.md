**Note:** auto-generated from comments in: ./target/package_gen.cmake

## package_gen.cmake:

Target for packaging and installing artifacts from modules. Defined as a standard target pattern.
This means that the following functions are defined:

- `add_global_target`: adds a global target 'package_gen'
- `add_module_target`: adds sub-targets for '<MODULE_NAME>_package_gen'


## Package function `add_global_target`:

Add target for the `package` custom target. Register a global 'package' target and a 'prepackage'
target that creates needed packaging directories.

- **TARGET_NAME:** target name to be generated


## Package function `add_module_target`:

Adds a module level packaging target that packages module artifacts.

- **MODULE_NAME:** name of the module
- **TARGET_NAME:** name of target to produce
- **GLOBAL_TARGET_NAME:** name of produced global target
- **AC_INPUTS:** list of autocoder inputs
- **SOURCE_FILES:** list of source file inputs
- **AC_OUTPUTS:** list of autocoder outputs
- **MOD_DEPS:** module dependencies of the target


