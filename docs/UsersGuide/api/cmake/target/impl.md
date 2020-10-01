**Note:** auto-generated from comments in: ./target/impl.cmake

## impl.cmake:

This target invokes the code template generation available as part of the fprime autocoder. It implements
the target interface described here: [Targets](Targets.md).

- `add_module_target`: adds sub-targets for '<MODULE_NAME>_impl'


## Impl function `add_module_target`:

Adds a module-by-module target for producing implementations. Take in the Ai.xml file and produces a set template
files.

- **MODULE_NAME:** name of the module
- **TARGET_NAME:** name of target to produce
- **AC_INPUTS:** list of autocoder inputs
- **SOURCE_FILES:** list of source file inputs
- **AC_OUTPUTS:** list of autocoder outputs


