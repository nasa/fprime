**Note:** auto-generated from comments in: ./target/prescan.cmake

## `add_global_target`:

The default implementation defines the target using `add_custom_target` and nothing more.


## `add_global_target`:

Used to clear the cache files before the prescan process runs module-to-module.


## `add_deployment_target`:

Defers to add_module_target for the prescan target. Nothing else special is done.


## `add_module_target`:

Used to detect scanned modules and inputs during the prescan.  These detected outputs are placed in the cache files
that the prescan builds.

- **MODULE:** name of the module being processed
- **TARGET:** unused
- **SOURCE:** list of source file inputs straight from the CMakeList.txt setup
- **DEPENDENCIES_DEFUNCT:** not really functioning in prescan. DO NOT USE.


