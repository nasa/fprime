**Note:** auto-generated from comments in: ./implementation.cmake

## implementation.cmake:

Functions used to handle different registered implementations of various cmake packages within F´. The process breaks
down as follows:
  1. A module will "require" an implementation X
  2. A number of modules will "register" as an implementation of X
  3. A platform package will "choose" a single implementation for X
  4. Linked modules (executable, deployment, unit-test) may override and "choose" an alternate implementation for X

For example: Os requires an implementation of Os_File. Os_File_Posix implements Os_File. The Linux platform chooses
Os_File_Posix as the implementation of Os_File. This applies to all linked modules in the system. Os_Baremetal_ut may
then override this choose using Os_File_MicroFs instead.

> All required implementations apply system-wide. Platforms specifically disallowing use of a given package should
> "choose" the special implementation "None" to bypass the check.

Internally this uses several properties. Each of these properties are GLOBAL such that these may be set before the
various targets have been explicitly created. These pro




## Function `remap_implementation_choices`:

When a target is declared with a different name than the module name, we need to pull the implementation choices into
a property for the declared target instead of the inferred target. This is done by reading each implementation choice
and setting the new property.

Args:
 INFERRED: inferred target name (FPRIME_CURRENT_MODULE)
 DECLARED: real target name


## Function `setup_executable_implementations`:

Sets up all the needed implementations for the supplied module. This looks up the global set of required
implementations and delegates each to `setup_executable_implementation` for final setup.

Args:
   MODULE: module to setup implementation choices for


## Function `setup_executable_implementation`:

Sets up the given implementation for the given module. Ensures that choices have been made and linking is setup
without causing global link dependencies.

Args:
   MODULE: module to setup implementation choices for


