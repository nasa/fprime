""" ini-to-stdio.py:

Loads fprime style ini files into a format CMake can process.
"""
import argparse
import sys
from functools import partial

from pathlib import Path
from typing import List

from fprime.fbuild.settings import IniSettings


def print_setting(setting: str, value: str = "", ending: str = ";"):
    """Print a setting for CMake

    Prints a setting for CMake using the format 'SETTING=VALUE;' producing a CMake list of settings.

    Args:
         setting: name of setting in cmake
         value: value to provide to cmake
         ending: ending of the print line
    """
    value = str(value).replace(";", "\\;")
    print(f"{setting}={value}", end=ending)


def print_list_settings(items: List[str]):
    """Print a list of settings of form SETTING=VALUE"""
    for item in items:
        splits = item.strip().split("=")
        if splits[0] != "":
            print_setting(*splits)


CMAKE_NEEDED_SETTINGS = {
    "framework_path": partial(print_setting, "FPRIME_FRAMEWORK_PATH"),
    "project_root": partial(print_setting, "FPRIME_PROJECT_ROOT"),
    "config_directory": partial(print_setting, "FPRIME_CONFIG_DIR"),
    "library_locations": lambda value: print_setting(
        "FPRIME_LIBRARY_LOCATIONS", ";".join(str(item) for item in value)
    ),
    "default_cmake_options": lambda value: print_list_settings(value.split("\n")),
    "ac_constants": partial(print_setting, "FPRIME_AC_CONSTANTS_FILE"),
    # Sets two settings from install dest: fprime and cmake settings
    "install_destination": partial(print_setting, "CMAKE_INSTALL_PREFIX"),
}


def main():
    """Do the thing."""
    parser = argparse.ArgumentParser()
    parser.add_argument("settings", type=Path, help="Path to settings.ini")
    parser.add_argument(
        "toolchain",
        nargs="?",
        type=Path,
        default=Path("native"),
        help="Path to toolchain file",
    )

    args_ns = parser.parse_args()
    loaded_settings = IniSettings.load(
        args_ns.settings, str(args_ns.toolchain.stem), False
    )
    loaded_settings_ut = IniSettings.load(
        args_ns.settings, str(args_ns.toolchain.stem), True
    )

    for setting, handler in CMAKE_NEEDED_SETTINGS.items():
        try:
            setting_value = loaded_settings[setting]
            ut_setting_value = loaded_settings_ut[setting]

            assert (
                setting_value == ut_setting_value
            ), f"CMake can only parse unittest independent settings"
            output = loaded_settings[setting]
            handler(output)
        except KeyError as key_error:
            print(
                f"[WARNING] Failed to load settings.ini field {key_error}. Update fprime-util.",
                end=";",
            )
    # Print the last setting with no ending to prevent null-entry at list end
    print_setting("FPRIME_SETTINGS_FILE", args_ns.settings, ending="")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception as exc:
        print(f"{exc}", file=sys.stderr)
    sys.exit(1)
