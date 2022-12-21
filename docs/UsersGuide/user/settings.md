# `settings.ini`: Build Settings Configuration

In many circumstances, it is useful to set default values for the build as well as configure external
locations for F´ to use external code. The `settings.ini` file allows users to set various settings
to control the build.

In this document:
- [`settings.ini` Settings](#settingsini-settings)
    - [`fprime` Section](#fprime-section)
    - [Example `settings.ini`](#example-settingsini)
    - [Example Environment Ini File](#example-environment-ini-file)

**WARNING:** The changes to the `settings.ini` file only take effect during the `fprime-util
generate` step, and thus builds **must** be regenerated after a change.

## `settings.ini` Settings

The `settings.ini` file is written in the INI format as interpreted by the default settings of the
Python `configparser` module. Should a user were to reference another key, the `%(other key)s` syntax should be used. The full format description is available here:

https://docs.python.org/3/library/configparser.html#supported-ini-file-structure

This file is expected in the directory of an F´ deployment and affects only the deployment it is
defined in. This directory is the same directory that contains the project's base `CMakeLists.txt`
(has a `project()` call).

**WARNING:** The changes to the `settings.ini` file only take effect during the `fprime-util
generate` step, and thus builds **must** be regenerated after a change.

### `fprime` Section

The `[fprime]` section defines settings for the F´ build.

These settings include:

- `project_root`: path to the root of the fprime project
- `framework_path`: Path to the F´ framework root
- `library_locations`: Paths to additional F´ libraries and components. Multiple paths can be
  specified with the `:` separator. Ex: `../library1:../library2`
- `default_toolchain`: Default platform to build against. Defaults to `native`, or the host
  computer platform.
- `default_ut_toolchain`: Default platform to build unit tests against. Defaults to `native`, or the host
  computer platform.
- `environment_file`: An ini file that can be used to set environmental variables during the build
  process.
- `ac_constants`: Path to autocode constants ini file.
- `config_directory`: Path to configuration header directory.

### Platform Sections

Some settings may be overridden for specific platforms using specific platform sections. These sections
have the same name as the platform and may set the following settings:

1. `config_directory`
2. `ac_constants`
3. `install_destination`
4. `environment_file`

These settings only apply when building for the specified platform.

### Example `settings.ini`

This `settings.ini` file comes from the [fprime-sphinx](https://github.com/fprime-community/fprime-sphinx)
deployment, which is a standard standalone deployment where the F´ framework and libraries are included
as git submodules at the top level.

```ini
[fprime]
project_root: ..
framework_path: ../fprime
library_locations: ../fprime-vxworks:../fprime-sphinx-drivers:../fprime-jplffs
default_toolchain: gr712-vxworks6
environment_file: ../fprime-vxworks/cmake/env/VxWorks-GR712.ini
ac_constants: ./Cfg/AcConstants.ini
config_directory: Cfg

[gr712-vxworks6]
config_directory: Cfg_gr712
```

### Example Environment Ini File

The format for the environment file option is similar to `settings.ini`
Each key in the file will be set as an environmental variable for the project build.

```ini
[environment]
WIND_BASE=/opt/WindRiver/vxworks-6.7
LINK_BIN_PRE=/opt/sparc-wrs-vxworks/bin/ccsparc
LINK_BIN_PRE_FLAGS=-r -nostdlib -Wl,-X
LINK_BIN_PRE_TO=-o
```