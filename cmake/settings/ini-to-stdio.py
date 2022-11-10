import argparse
import sys

from pathlib import Path
from fprime.fbuild.settings import IniSettings

CMAKE_NEEDED_SETTINGS = {
    "framework_path",
    "library_locations",
    "project_root",
    "config_directory",
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

    for setting in CMAKE_NEEDED_SETTINGS:
        setting_value = loaded_settings[setting]
        ut_setting_value = loaded_settings_ut[setting]

        assert (
            setting_value == ut_setting_value
        ), f"CMake can only parse unittest independent settings"
        output = loaded_settings[setting]
        print(
            f"{setting}|{':'.join([str(item) for item in output]) if isinstance(output, list) else output}"
        )
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception as exc:
        print(f"{exc}", file=sys.stderr)
    sys.exit(1)
