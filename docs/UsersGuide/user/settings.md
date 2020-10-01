# `settings.ini`: Build Settings Configuration

In many circumstances it is useful to set default values for the build as well as configure external locations for F´ to
use external code. The `settings.ini` file allows users to set various settings to control the build.  **Note:**
settings.ini only adjusts the build when running through `fprime-util`.  These settings can be supplied directly to
CMake using -D flags as described here: [CMake Settings](../dev/cmake-settings.md), but it is not recommended to use
CMake directly.

In this document:
- [`settings.ini` Settings](#settingsini-settings)
    - [`fprime` Section](#fprime-section)
    - [`environment` Section](#environment-section)
- [Environment File Settings](#environment-file-settings)

**WARNING:** The changes to the `settings.ini` file only take effect during the `fprim-util generate` step, and thus
builds **must** be regenerated after a change.

## `settings.ini` Settings

The `settings.ini` file is written in the INI format as interpreted by the default settings of the Python `configparser`
module. This file may optionally define two sections `[fprime]` and `[environment]` whose keys are explained below.
Should a user with to reference another key, use the `%(other key)s` syntax. Full format description is available here:

https://docs.python.org/3/library/configparser.html#supported-ini-file-structure

This file is expected in the directory of the an F´ deployment and affects only the deployment it is defined in. This
directory is the same directory that contains the project's base `CMakeLists.txt` (has a `project()` call).

**WARNING:** The changes to the `settings.ini` file only take effect during the `fprim-util generate` step, and thus
builds **must** be regenerated after a change.

### `fprime` Section

The F´ section defines settings for the F´ build. These settings include:

1. framework_path
project_root
ac_constants
config_directory
environment_file
library_locations
default_toolchain", fallback="native"
        

### `environment` Section


## Environment File Settings